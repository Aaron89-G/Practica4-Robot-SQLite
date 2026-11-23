#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QApplication>

// --- LIBRERÍAS NUEVAS PARA LA PRÁCTICA 4 ---
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox> // Para E5: ID de Nota
#include <QLineEdit> // Para E5: Texto de Nota

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

    QTimer *cronometro = new QTimer(this);
    connect(cronometro, &QTimer::timeout, this, &MainWindow::loop);
    cronometro->start(1000); // Latido cada 1s

    // Temporizador para pedir la distancia al HC-SR04
    QTimer *timerDistancia = new QTimer(this);
    connect(timerDistancia, &QTimer::timeout, this, &MainWindow::pedirDistancia);
    timerDistancia->start(500);

    // ---- Configuración de Direccionales ----
    connect(&timerDireccionales, &QTimer::timeout, this, [this](){
        static bool encendido = false;
        encendido = !encendido;
        if(socket.state() == QAbstractSocket::ConnectedState) {
            socket.sendTextMessage("{\"tipo\":\"luz\", \"pin\":9, \"estado\":" + QString::number(encendido ? 1 : 0) + "}");
            QTimer::singleShot(20, this, [this, encendido]() {
                if(socket.state() == QAbstractSocket::ConnectedState) {
                    socket.sendTextMessage("{\"tipo\":\"luz\", \"pin\":7, \"estado\":" + QString::number(encendido ? 1 : 0) + "}");
                }
            });
        }
    });

    // ---- Configuración del Servo ----
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
    ejeDistancia->setRange(0, 50);
    grafica->addAxis(ejeTiempo, Qt::AlignBottom);
    grafica->addAxis(ejeDistancia, Qt::AlignLeft);
    serie->attachAxis(ejeTiempo);
    serie->attachAxis(ejeDistancia);
    ui->graficaPot->setChart(grafica);
    ui->graficaPot->setRenderHint(QPainter::Antialiasing);

    // ==========================================
    // PRÁCTICA 4 - E1: CREAR PESTAÑA TEMPERATURA
    // ==========================================
    auto *pestanas = new QTabWidget(this);
    pestanas->addTab(ui->centralwidget, "Robot");

    auto *pagTemp = new QWidget;
    m_capaTemp = new QVBoxLayout(pagTemp);
    pestanas->addTab(pagTemp, "Temperatura");
    setCentralWidget(pestanas);

    // Controles superiores (E1 y E3)
    QHBoxLayout *capaControles = new QHBoxLayout();
    QPushButton *btnLeerTemp = new QPushButton("Leer temperatura", this);
    m_temperatura = new QLabel("--- °C", this);
    m_chkAutoTemp = new QCheckBox("Guardar cada 10 s", this);

    capaControles->addWidget(btnLeerTemp);
    capaControles->addWidget(m_temperatura);
    capaControles->addWidget(m_chkAutoTemp);
    m_capaTemp->addLayout(capaControles);

    // ==========================================
    // PRÁCTICA 4 - E4: TABLA DE HISTORIAL
    // ==========================================
    m_tabla = new QTableWidget(0, 4, this);
    m_tabla->setHorizontalHeaderLabels({"ID", "Fecha y Hora", "°C", "Nota"});
    m_tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QPushButton *btnCargarHistorial = new QPushButton("Cargar historial", this);
    m_total = new QLabel("Mediciones guardadas: 0", this);

    m_capaTemp->addWidget(m_tabla);
    m_capaTemp->addWidget(btnCargarHistorial);
    m_capaTemp->addWidget(m_total);

    // ==========================================
    // PRÁCTICA 4 - E5: AÑADIR NOTAS A MEDICIONES
    // ==========================================
    QHBoxLayout *capaNota = new QHBoxLayout();
    QLabel *lblId = new QLabel("ID:", this);
    m_id = new QSpinBox(this);
    m_id->setRange(0, 99999);
    m_id->setSpecialValueText("-"); // El 0 se verá como un guion

    QLabel *lblNota = new QLabel("Nota:", this);
    m_nota = new QLineEdit(this);
    QPushButton *btnGuardarNota = new QPushButton("Guardar nota", this);

    capaNota->addWidget(lblId);
    capaNota->addWidget(m_id);
    capaNota->addWidget(lblNota);
    capaNota->addWidget(m_nota);
    capaNota->addWidget(btnGuardarNota);
    m_capaTemp->addLayout(capaNota);

    // Conectar botón manual de temperatura
    connect(btnLeerTemp, &QPushButton::clicked, this, [this]() {
        if(socket.state() == QAbstractSocket::ConnectedState) {
            socket.sendTextMessage("{\"tipo\":\"leer_dht\"}");
        }
    });

    // Conectar Auto-Guardado (E3)
    m_timerAutoTemp = new QTimer(this);
    connect(m_chkAutoTemp, &QCheckBox::toggled, this, [this](bool checked){
        if(checked) m_timerAutoTemp->start(10000);
        else m_timerAutoTemp->stop();
    });
    connect(m_timerAutoTemp, &QTimer::timeout, this, [this](){
        if(socket.state() == QAbstractSocket::ConnectedState) {
            socket.sendTextMessage("{\"tipo\":\"leer_dht\"}");
        }
    });

    // Conectar botón "Cargar Historial" (E4)
    connect(btnCargarHistorial, &QPushButton::clicked, this, [this]() {
        QSqlQuery consulta("SELECT id, fecha_hora, temperatura, nota FROM mediciones ORDER BY id DESC LIMIT 50");
        if (!consulta.isActive()) {
            registrarEvento("SELECT falló: " + consulta.lastError().text());
            return;
        }
        m_tabla->setRowCount(0);
        while (consulta.next()) {
            const int fila = m_tabla->rowCount();
            m_tabla->insertRow(fila);
            m_tabla->setItem(fila, 0, new QTableWidgetItem(consulta.value(0).toString()));
            m_tabla->setItem(fila, 1, new QTableWidgetItem(consulta.value(1).toString()));
            m_tabla->setItem(fila, 2, new QTableWidgetItem(QString::number(consulta.value(2).toDouble(), 'f', 1)));
            m_tabla->setItem(fila, 3, new QTableWidgetItem(consulta.value(3).toString()));
        }
        m_tabla->resizeColumnsToContents();

        QSqlQuery total("SELECT COUNT(*) FROM mediciones");
        if (total.next()) {
            m_total->setText("Mediciones guardadas: " + total.value(0).toString());
        }
    });

    // Conectar el click en la tabla para copiar el ID (E5)
    connect(m_tabla, &QTableWidget::cellClicked, this, [this](int fila, int) {
        m_id->setValue(m_tabla->item(fila, 0)->text().toInt());
    });

    // Conectar el botón de Guardar nota (E5)
    connect(btnGuardarNota, &QPushButton::clicked, this, [this, btnCargarHistorial]() {
        if (m_id->value() == 0) {
            registrarEvento("Primero toca una fila");
            return;
        }

        QSqlQuery consulta;
        consulta.prepare("UPDATE mediciones SET nota = :nota WHERE id = :id");
        consulta.bindValue(":nota", m_nota->text());
        consulta.bindValue(":id", m_id->value());

        if (!consulta.exec()) {
            registrarEvento("UPDATE falló: " + consulta.lastError().text());
            return;
        }

        registrarEvento(QString("Filas actualizadas: %1").arg(consulta.numRowsAffected()));

        m_nota->clear();
        m_id->setValue(0);

        // Recargar la tabla (E4)
        QMetaObject::invokeMethod(btnCargarHistorial, "clicked");
    });

    // ==========================================
    // PRÁCTICA 4 - E2: INICIALIZACIÓN DE LA BASE DE DATOS
    // ==========================================
    const QString carpeta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(carpeta);
    const QString ruta = carpeta + "/temperatura_0227.db";
    const bool yaExistia = QFile::exists(ruta);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(ruta);
    if (!db.open()) {
        registrarEvento("No se abrió la base: " + db.lastError().text());
        return;
    }

    QSqlQuery consulta;
    if (!consulta.exec("CREATE TABLE IF NOT EXISTS mediciones ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "fecha_hora TEXT NOT NULL, "
                       "temperatura REAL NOT NULL, "
                       "fuente TEXT NOT NULL, "
                       "nota TEXT)")) {
        registrarEvento("No se creó la tabla: " + consulta.lastError().text());
        return;
    }

    registrarEvento((yaExistia ? "Base encontrada: " : "Base nueva creada: ") + ruta);

    // Carga inicial de la tabla al abrir la app
    emit btnCargarHistorial->clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_botonConectar_clicked()
{
    if (socket.state() == QAbstractSocket::ConnectedState) {
        socket.close();
        return;
    }
    QString url = "ws://" + ui->lineEditIp->text() + ":" + QString::number(ui->spinPuerto->value()) + "/";
    ui->etiquetaEstado->setText("Conectando...");
    socket.open(QUrl(url));
}

void MainWindow::loop(){
    if(socket.state() == QAbstractSocket::ConnectedState) {
        socket.sendTextMessage("{\"tipo\":\"latido\"}");
    }
}

void MainWindow::pedirDistancia(){
    if(socket.state() == QAbstractSocket::ConnectedState) {
        socket.sendTextMessage("{\"tipo\":\"leer_distancia\"}");
    }
}

void MainWindow::alConectar(){
    ui->etiquetaEstado->setText("Conectado");
    ui->botonConectar->setText("Desconectar");
    registrarEvento("Conexión exitosa al robot");
    serie->clear();
    reloj.start();
}

void MainWindow::alDesconectar(){
    ui->etiquetaEstado->setText("Desconectado");
    ui->botonConectar->setText("Conectar");
    registrarEvento("Robot desconectado");

    if(m_chkAutoTemp) m_chkAutoTemp->setChecked(false);
    if(m_timerAutoTemp) m_timerAutoTemp->stop();
}

void MainWindow::enviarJSON(const QString &mensaje){
    if(socket.state() != QAbstractSocket::ConnectedState) return;
    socket.sendTextMessage(mensaje);
}

void MainWindow::alRecibirMensaje(const QString &mensaje){
    QJsonDocument doc = QJsonDocument::fromJson(mensaje.toUtf8());
    QJsonObject obj = doc.object();
    QString tipo = obj["tipo"].toString();

    // --- PRÁCTICA 4 - E1 y E3: LEER Y GUARDAR TEMPERATURA ---
    if (tipo == "dht_lectura") {
        registrarEvento("RX " + mensaje);
        const QJsonValue t = obj.value("temperatura");

        if (!obj.value("dht_ok").toBool() || !t.isDouble()) {
            registrarEvento("Lectura inválida: no se guarda");
            return;
        }

        m_temperatura->setText(QString::number(t.toDouble(), 'f', 1) + " °C");
        guardarTemperatura(t.toDouble());
    }
    // --- LECTURA DE DISTANCIA ---
    else if(tipo == "distancia_lectura") {
        double dist = obj["d"].toDouble();
        double t = reloj.elapsed() / 1000.0;

        serie->append(t, dist);
        if (serie->count() > 100) serie->remove(0);
        if (t > 10) ejeTiempo->setRange(t - 10, t);

        if (dist > 0 && dist <= 10.0) {
            if(ui->etiquetaEstado) {
                ui->etiquetaEstado->setStyleSheet("color: white; background-color: red; font-weight: bold; border-radius: 5px;");
                ui->etiquetaEstado->setText(" ¡PELIGRO! Colisión a " + QString::number(dist, 'f', 1) + " cm ");
            }
            QApplication::beep();
            static double distAnterior = 100.0;
            if (distAnterior > 10.0) registrarEvento("Alerta de impacto (" + QString::number(dist, 'f', 1) + " cm)");
            distAnterior = dist;
        } else {
            if(ui->etiquetaEstado) {
                ui->etiquetaEstado->setStyleSheet("");
                ui->etiquetaEstado->setText("Distancia: " + QString::number(dist, 'f', 1) + " cm");
            }
            static double distAnterior = 100.0;
            distAnterior = dist;
        }
    }
}

// ==========================================
// PRÁCTICA 4 - E3 y E4: FUNCIÓN GUARDAR TEMPERATURA
// ==========================================
static const char *FUENTE = "simulada";

void MainWindow::guardarTemperatura(double celsius)
{
    const double c1 = qRound(celsius * 10) / 10.0;

    QSqlQuery consulta;
    consulta.prepare("INSERT INTO mediciones (fecha_hora, temperatura, fuente) VALUES (:fecha_hora, :temperatura, :fuente)");
    consulta.bindValue(":fecha_hora", QDateTime::currentDateTime().toString(Qt::ISODate));
    consulta.bindValue(":temperatura", c1);
    consulta.bindValue(":fuente", QString::fromUtf8(FUENTE));

    if (!consulta.exec()) {
        registrarEvento("INSERT falló: " + consulta.lastError().text());
        return;
    }

    registrarEvento(QString("Guardada %1 °C con id %2").arg(c1, 0, 'f', 1).arg(consulta.lastInsertId().toString()));

    // Recargar la tabla (E4)
    QSqlQuery selectQuery("SELECT id, fecha_hora, temperatura, nota FROM mediciones ORDER BY id DESC LIMIT 50");
    if (selectQuery.isActive() && m_tabla) {
        m_tabla->setRowCount(0);
        while (selectQuery.next()) {
            const int fila = m_tabla->rowCount();
            m_tabla->insertRow(fila);
            m_tabla->setItem(fila, 0, new QTableWidgetItem(selectQuery.value(0).toString()));
            m_tabla->setItem(fila, 1, new QTableWidgetItem(selectQuery.value(1).toString()));
            m_tabla->setItem(fila, 2, new QTableWidgetItem(QString::number(selectQuery.value(2).toDouble(), 'f', 1)));
            m_tabla->setItem(fila, 3, new QTableWidgetItem(selectQuery.value(3).toString()));
        }
    }
    QSqlQuery total("SELECT COUNT(*) FROM mediciones");
    if (total.next() && m_total) {
        m_total->setText("Mediciones guardadas: " + total.value(0).toString());
    }
}

// ---- CONTROLES DE MOVIMIENTO ----
void MainWindow::on_pushButton_3_clicked() {
    if (emoteActivo || socket.state() != QAbstractSocket::ConnectedState) return;
    registrarEvento("Moviendo: Atrás");
    socket.sendTextMessage("{\"tipo\":\"mover\", \"accion\":\"retroceder\", \"ms\": 150}");
}
void MainWindow::on_pushButton_4_clicked() {
    if (emoteActivo || socket.state() != QAbstractSocket::ConnectedState) return;
    registrarEvento("Moviendo: Adelante");
    socket.sendTextMessage("{\"tipo\":\"mover\", \"accion\":\"avanzar\", \"ms\": 150}");
}
void MainWindow::on_pushButton_2_clicked() {
    if (emoteActivo || socket.state() != QAbstractSocket::ConnectedState) return;
    registrarEvento("Moviendo: Izquierda");
    socket.sendTextMessage("{\"tipo\":\"mover\", \"accion\":\"izquierda\", \"ms\": 150}");
}
void MainWindow::on_pushButton_clicked() {
    if (emoteActivo || socket.state() != QAbstractSocket::ConnectedState) return;
    registrarEvento("Moviendo: Derecha");
    socket.sendTextMessage("{\"tipo\":\"mover\", \"accion\":\"derecha\", \"ms\": 150}");
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
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
        ui->deslizadorAngulo->setValue(anguloPendiente);
        return;
    }
    if(ui->etiquetaPedido) ui->etiquetaPedido->setText(QString::number(value) + "°");
    anguloPendiente = value;
    if (!temporizadorServo.isActive()) {
        temporizadorServo.start();
    }
}

// ==========================================
// EXTRAS
// ==========================================
void MainWindow::registrarEvento(const QString &texto) {
    if(ui->listaEventos) {
        QString timestamp = QTime::currentTime().toString("hh:mm:ss");
        ui->listaEventos->insertItem(0, "[" + timestamp + "] " + texto);
    }
}

void MainWindow::on_btnCuchau_clicked() {
    if (emoteActivo) return;
    emoteActivo = true;
    registrarEvento("Emote: Cuchau ejecutado");
    enviarJSON("{\"tipo\":\"mover\", \"accion\":\"izquierda\", \"ms\": 150}");
    QTimer::singleShot(200, this, [this](){ enviarJSON("{\"tipo\":\"luz\", \"pin\":9, \"estado\":1}"); });
    QTimer::singleShot(400, this, [this](){ enviarJSON("{\"tipo\":\"luz\", \"pin\":9, \"estado\":0}"); });
    QTimer::singleShot(450, this, [this](){ emoteActivo = false; });
}

void MainWindow::on_btnMiedo_clicked() {
    if (emoteActivo) return;
    emoteActivo = true;
    registrarEvento("Emote: Miedo ejecutado");
    for(int i = 0; i < 3; i++) {
        QTimer::singleShot(i * 250, this, [this](){ enviarJSON("{\"tipo\":\"mover\", \"accion\":\"retroceder\", \"ms\": 100}"); });
    }
    QTimer::singleShot(750, this, [this](){ emoteActivo = false; });
}

void MainWindow::on_btnNoNo_clicked() {
    if (emoteActivo) return;
    emoteActivo = true;
    registrarEvento("Emote: No-No ejecutado");
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
        timerDireccionales.start(400);
    } else {
        registrarEvento("Intermitentes apagadas");
        timerDireccionales.stop();
        enviarJSON("{\"tipo\":\"luz\", \"pin\":9, \"estado\":0}");
        QTimer::singleShot(20, this, [this](){ enviarJSON("{\"tipo\":\"luz\", \"pin\":7, \"estado\":0}"); });
    }
}