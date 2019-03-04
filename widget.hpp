#ifndef QFRAMELESSWINDOW_WIDGET_HPP
#define QFRAMELESSWINDOW_WIDGET_HPP

#include <QtWidgets/QWidget>

class QPushButton;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;

namespace qt_extended {

  class title_bar : public QWidget {
    
    Q_OBJECT

    struct _ui;

    public:

      QHBoxLayout *main_layout;

      explicit title_bar(QWidget *parent) noexcept;
      const _ui& get_ui() const noexcept;

    protected:

      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void paintEvent(QPaintEvent *event);


      static const int title_height{45};
      
    private:

      struct _ui {
        QLabel *title;
        QPushButton *close_button;
        QPushButton *maximize_button;
        QPushButton *minimize_button;
      } ui;
      
      QPoint cursor;
      QWidget *parent;
      bool is_maximized;
      
  };
  
  class widget : public QWidget {
    Q_OBJECT

    public:

      QVBoxLayout *main_layout;

      explicit widget(QWidget *parent = nullptr) noexcept;

      const title_bar* get_title_bar() const noexcept;
    
    protected:

      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void mouseReleaseEvent(QMouseEvent *event);
      void paintEvent(QPaintEvent *event);

      static const int resize_region{5};
    
    private: 

      title_bar *w_title_bar;

 
      enum edge: int16_t {
        none = 0x0000,
        left = 0x0001,
        right = 0x0010,
        top = 0x0100,
        bottom = 0x1000
      };
      int16_t current_edge;

      int16_t get_edge(const QPoint &point);
      void set_cursor(int16_t edge_pos);
  };
}

#endif
