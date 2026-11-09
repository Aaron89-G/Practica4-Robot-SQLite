#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QApplication> // <--- PARA EL SONIDO DE ALARMA (beep)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Aarón Emiliano Aguayo Castañeda - 220530227");

    setFocusPolicy(Qt::StrongFocus);
    if (centralWidget()) centralWidget()->setFocusPolicy(Qt::StrongFocus);

    // Fuerza el inicio del deslizador en 90°
    if(ui->deslizadorAngulo) {
        ui->deslizadorAngulo->setMinimum(0);
        ui->deslizadorAngulo->setMaximum(180);
        ui->deslizadorAngulo->setValue(90);
    }

    // Conectamos las señales del WebSocket
    connect(&socket, &QWebSocket::connected, this, &MainWindow::alConectar);
    connect(&socket, &QWebSocket::disconnected, this, &MainWindow::alDesconectar);
    connect(&socket, &QWebSocket::textMessageReceived, this, &MainWindow::alRecibirMensaje);
    connect(&socket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError){
        if(ui->etiquetaEstado) ui->etiquetaEstado->setText("Error: " + socket.errorString());
    });

    QTimer *cronometro = new QTimer(this);
    connect(cronometro, &QTimer::timeout, this, &MainWindow::loop);
    cronometro->start(1000); // Latido cada 1s

    // Temporizador para pedir la distancia al HC-SR04
    QTimer *timerDistancia = new QTimer(this);
    connect(timerDistancia, &QTimer::timeout, this, &MainWindow::pedirDistancia);
    timerDistancia->start(250); // Solicita distancia 4 veces por segundo

    // ---- Configuración de Direccionales (Desfasadas) ----
    connect(&timerDireccionales, &QTimer::timeout, this, [this](){
        static bool encendido = false;
        encendido = !encendido;
        if(socket.state() == QAbstractSocket::ConnectedState) {
            // Enciende/apaga el LED Izquierdo
            socket.sendTextMessage("{\"tipo\":\"luz\", \"pin\":9, \"estado\":" + QString::number(encendido ? 1 : 0) + "}");

            // Espera 20ms para no ahogar al ESP32, y manda el LED Derecho
            QTimer::singleShot(20, this, [this]() {
                if(socket.state() == QAbstractSocket::ConnectedState) {
                    socket.sendTextMessage("{\"tipo\":\"luz\", \"pin\":7, \"estado\":" + QString::number(encendido ? 1 : 0) + "}");
                }
            });
        }
    });

    // ---- Configuración del temporizador del Servo ----
    temporizadorServo.setInterval(50);
    connect(&temporizadorServo, &QTimer::timeout, this, [this]() {
        if (anguloPendiente < 0) {
            temporizadorServo.stop();
            return;
        }
        QString mensaje = "{\"tipo\":\"servo\", \"angulo\":" + QString::number(anguloPendiente) + "}";
        if(socket.state() == QAbstractSocket::ConnectedState) {
            socket.sendTextMessage(mensaje);
        }
        anguloPendiente = -1;
    });

    // ---- Configuración de la Gráfica ----
    grafica->addSeries(serie);
    grafica->legend()->hide();
    grafica->setTitle("Distancia HC-SR04");

    ejeTiempo->setTitleText("Tiempo (s)");
    ejeTiempo->setRange(0, 10);

    ejeDistancia->setTitleText("Distancia (cm)");
    ejeDistancia->setRange(0, 50); // Rango inicial de 0 a 50 cm

    grafica->addAxis(ejeTiempo, Qt::AlignBottom);
    grafica->addAxis(ejeDistancia, Qt::AlignLeft);
    serie->attachAxis(ejeTiempo);
    serie->attachAxis(ejeDistancia);

    ui->graficaPot->setChart(grafica);
    ui->graficaPot->setRenderHint(QPainter::Antialiasing);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_botonConectar_clicked()
{
    // Escudo contra doble toque táctil o estado "Conectando..."
    if (socket.state() != QAbstractSocket::UnconnectedState) {
        socket.close();
        return;
    }

    // URL idéntica a la del profesor (sin la "/" al final)
    QString url = "ws://" + ui->lineEditIp->text().trimmed() + ":" + QString::number(ui->spinPuerto->value());

    ui->etiquetaEstado->setText("Conectando...");
    socket.open(QUrl(url));
}

void MainWindow::loop(){
    if(socket.state() == QAbstractSocket::ConnectedState) {
        socket.sendTextMessage("{\"tipo\":\"latido\"}");
    }
}

// ---- SOLICITUD DE DISTANCIA ----
void MainWindow::pedirDistancia(){
    if(socket.state() == QAbstractSocket::ConnectedState) {
        socket.sendTextMessage("{\"tipo\":\"leer_distancia\"}");
    }
}

void MainWindow::alConectar(){
    ui->etiquetaEstado->setText("Conectado");
    ui->botonConectar->setText("Desconectar");
    registrarEvento("Conexión exitosa al robot");

    // Reiniciamos la gráfica al conectar
    serie->clear();
    reloj.start();
}

void MainWindow::alDesconectar(){
    ui->etiquetaEstado->setText("Desconectado");
    ui->botonConectar->setText("Conectar");
    registrarEvento("Robot desconectado");
}

void MainWindow::enviarJSON(const QString &mensaje){
    if(socket.state() != QAbstractSocket::ConnectedState) return;
    socket.sendTextMessage(mensaje);
}

void MainWindow::alRecibirMensaje(const QString &mensaje){
    // Convertimos el mensaje a JSON
    QJsonDocument doc = QJsonDocument::fromJson(mensaje.toUtf8());
    QJsonObject obj = doc.object();
    QString tipo = obj["tipo"].toString();

    // Si es una lectura de distancia, la graficamos
    if(tipo == "distancia_lectura") {
        double dist = obj["d"].toDouble();
        double t = reloj.elapsed() / 1000.0; // Segundos desde que se conectó

        serie->append(t, dist);

        // Mantenemos solo los últimos 100 puntos (aprox. 25 segundos) en memoria
        if (serie->count() > 100) {
            serie->remove(0);
        }
        // Desplazamos el eje X (efecto de cinta de correr)
        if (t > 10) {
            ejeTiempo->setRange(t - 10, t);
        }

        // EXTRA 2: Alarma Visual y Sonora a 10 cm
        if (dist > 0 && dist <= 10.0) {
            if(ui->etiquetaEstado) {
                ui->etiquetaEstado->setStyleSheet("color: white; background-color: red; font-weight: bold; border-radius: 5px;");
                ui->etiquetaEstado->setText(" ¡PELIGRO! Colisión a " + QString::number(dist, 'f', 1) + " cm ");
            }
            QApplication::beep(); // Sonido de alerta de PC

            static double distAnterior = 100.0;
            if (distAnterior > 10.0) registrarEvento("Alerta de impacto (" + QString::number(dist, 'f', 1) + " cm)");
            distAnterior = dist;
        } else {
            if(ui->etiquetaEstado) {
                ui->etiquetaEstado->setStyleSheet("");
                ui->etiquetaEstado->setText("Distancia: " + QString::number(dist, 'f', 1) + " cm");
            }
            //static double distAnterior = 100.0;
            //distAnterior = dist;
        }
    }
}

// ---- CONTROLES DE MOVIMIENTO ----
void MainWindow::on_pushButton_3_clicked() {
    if (emoteActivo) return;
    registrarEvento("Moviendo: Atrás");
    enviarJSON("{\"tipo\":\"mover\", \"accion\":\"retroceder\", \"ms\": 150}");
}
void MainWindow::on_pushButton_4_clicked() {
    if (emoteActivo) return;
    registrarEvento("Moviendo: Adelante");
    enviarJSON("{\"tipo\":\"mover\", \"accion\":\"avanzar\", \"ms\": 150}");
}
void MainWindow::on_pushButton_2_clicked() {
    if (emoteActivo) return;
    registrarEvento("Moviendo: Izquierda");
    enviarJSON("{\"tipo\":\"mover\", \"accion\":\"izquierda\", \"ms\": 150}");
}
void MainWindow::on_pushButton_clicked() {
    if (emoteActivo) return;
    registrarEvento("Moviendo: Derecha");
    enviarJSON("{\"tipo\":\"mover\", \"accion\":\"derecha\", \"ms\": 150}");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    static qint64 ultimoMensaje = 0;
    qint64 ahora = QDateTime::currentMSecsSinceEpoch();
    if (ahora - ultimoMensaje < 100) return;
    ultimoMensaje = ahora;

    switch (event->key()) {
    case Qt::Key_Up: on_pushButton_4_clicked(); break;
    case Qt::Key_Down: on_pushButton_3_clicked(); break;
    case Qt::Key_Left: on_pushButton_2_clicked(); break;
    case Qt::Key_Right: on_pushButton_clicked(); break;
    default: QWidget::keyPressEvent(event); break;
    }
}

void MainWindow::on_deslizadorAngulo_valueChanged(int value) {
    if (emoteActivo) {
        ui->deslizadorAngulo->setValue(anguloPendiente); // Si hay un emote, no deja mover el slider manual
        return;
    }
    if(ui->etiquetaPedido) ui->etiquetaPedido->setText(QString::number(value) + "°");
    anguloPendiente = value;
    if (!temporizadorServo.isActive()) {
        temporizadorServo.start();
    }
}

// ==========================================
// EXTRA 3: HISTORIAL DE EVENTOS
// ==========================================
void MainWindow::registrarEvento(const QString &texto) {
    if(ui->listaEventos) {
        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        ui->listaEventos->insertItem(0, "[" + timestamp + "] " + texto);
    }
}

// ==========================================
// EXTRA 1: BARRA DE EMOTES Y DIRECCIONALES (SEGUROS)
// ==========================================
void MainWindow::on_btnCuchau_clicked() {
    if (emoteActivo) return;
    emoteActivo = true;

    registrarEvento("Emote: Cuchau ejecutado");

    // Paso 1: Mueve el carrito (150ms)
    enviarJSON("{\"tipo\":\"mover\", \"accion\":\"izquierda\", \"ms\": 150}");

    // Paso 2: Hasta que el motor termina (200ms), hace el guiño con el LED (sin servo)
    QTimer::singleShot(200, this, [this](){ enviarJSON("{\"tipo\":\"luz\", \"pin\":9, \"estado\":1}"); });
    QTimer::singleShot(400, this, [this](){ enviarJSON("{\"tipo\":\"luz\", \"pin\":9, \"estado\":0}"); });

    // Paso 3: Libera el candado
    QTimer::singleShot(450, this, [this](){ emoteActivo = false; });
}

void MainWindow::on_btnMiedo_clicked() {
    if (emoteActivo) return;
    emoteActivo = true;

    registrarEvento("Emote: Miedo ejecutado");

    // Tiembla con los motores hacia atrás
    for(int i = 0; i < 3; i++) {
        QTimer::singleShot(i * 250, this, [this](){ enviarJSON("{\"tipo\":\"mover\", \"accion\":\"retroceder\", \"ms\": 100}"); });
    }

    QTimer::singleShot(750, this, [this](){ emoteActivo = false; });
}

void MainWindow::on_btnNoNo_clicked() {
    if (emoteActivo) return;
    emoteActivo = true;

    registrarEvento("Emote: No-No ejecutado");

    // Dice "No" sacudiendo las llantas (izquierda-derecha, sin tocar el servo)
    QTimer::singleShot(0, this, [this](){ enviarJSON("{\"tipo\":\"mover\", \"accion\":\"izquierda\", \"ms\": 100}"); });
    QTimer::singleShot(150, this, [this](){ enviarJSON("{\"tipo\":\"mover\", \"accion\":\"derecha\", \"ms\": 100}"); });
    QTimer::singleShot(300, this, [this](){ enviarJSON("{\"tipo\":\"mover\", \"accion\":\"izquierda\", \"ms\": 100}"); });
    QTimer::singleShot(450, this, [this](){ enviarJSON("{\"tipo\":\"mover\", \"accion\":\"derecha\", \"ms\": 100}"); });

    QTimer::singleShot(600, this, [this](){ emoteActivo = false; });
}

void MainWindow::on_btnDireccionales_clicked() {
    direccionalesActivas = !direccionalesActivas;
    if(direccionalesActivas) {
        registrarEvento("Intermitentes encendidas");
        timerDireccionales.start(400); // Parpadeo cada 400ms
    } else {
        registrarEvento("Intermitentes apagadas");
        timerDireccionales.stop();
        enviarJSON("{\"tipo\":\"luz\", \"pin\":9, \"estado\":0}");
        QTimer::singleShot(20, this, [this](){ enviarJSON("{\"tipo\":\"luz\", \"pin\":7, \"estado\":0}"); });
    }
}