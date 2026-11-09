#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QTimer>
#include <QKeyEvent>
#include <QtCharts>
#include <QElapsedTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void alConectar();
    void alDesconectar();
    void enviarJSON(const QString &mensaje);
    void loop();
    void pedirDistancia();
    void alRecibirMensaje(const QString &mensaje);

    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

    void on_botonConectar_clicked();
    void on_deslizadorAngulo_valueChanged(int value);
    void keyPressEvent(QKeyEvent *event) override;

    // ---- SLOTS EXTRA ----
    void registrarEvento(const QString &texto);
    void on_btnCuchau_clicked();
    void on_btnMiedo_clicked();
    void on_btnNoNo_clicked();
    void on_btnDireccionales_clicked();

private:
    Ui::MainWindow *ui;
    QWebSocket socket;
    QTimer temporizadorServo;
    int anguloPendiente = -1;

    // ---- VARIABLES PARA EXTRA ----
    QTimer timerDireccionales;
    bool direccionalesActivas = false;
    bool emoteActivo = false; // Candado para evitar saturación del servo

    // ---- OBJETOS PARA LA GRÁFICA ----
    QChart *grafica = new QChart();
    QLineSeries *serie = new QLineSeries();
    QValueAxis *ejeTiempo = new QValueAxis();
    QValueAxis *ejeDistancia = new QValueAxis();
    QElapsedTimer reloj;
};
#endif // MAINWINDOW_H