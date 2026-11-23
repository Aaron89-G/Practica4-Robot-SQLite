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
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>

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

    void guardarTemperatura(double celsius);
    void actualizarFechas();
    void cargarHistorial();

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

    QVBoxLayout *m_capaTemp;
    QLabel *m_temperatura;

    QCheckBox *m_chkAutoTemp;
    QTimer *m_timerAutoTemp;

    QTableWidget *m_tabla;
    QLabel *m_total;

    QSpinBox *m_id;
    QLineEdit *m_nota;

    // ---- EXTRA P4: FILTRO POR FECHA Y GRÁFICA HISTÓRICA SQLITE ----
    QComboBox *m_comboFecha = nullptr;
    QLabel *m_resumenFecha = nullptr;
    QChart *m_graficaTemp = nullptr;
    QLineSeries *m_serieTemp = nullptr;
    QValueAxis *m_ejeIdTemp = nullptr;
    QValueAxis *m_ejeGradosTemp = nullptr;
    QChartView *m_vistaGraficaTemp = nullptr;
};
#endif // MAINWINDOW_H