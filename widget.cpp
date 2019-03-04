#include "widget.hpp"

#include <QtWidgets/QApplication>

#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>

#include <QtGui/QMouseEvent>


// for paintEvent
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOption>

namespace qt_extended {
  widget::widget(QWidget *parent) noexcept : QWidget(parent), 
                                             main_layout(new QVBoxLayout(this)),
                                             w_title_bar(new title_bar(this)),
                                             current_edge(edge::none) {
    setWindowFlag(Qt::FramelessWindowHint);
    setBackgroundRole(QPalette::Highlight);
    setMouseTracking(true);

    
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    main_layout->setAlignment(Qt::AlignTop);

    auto *title_wrapper = new QWidget;
    title_wrapper->setMouseTracking(true);
    title_wrapper->setProperty("class", "title_bar");
    title_wrapper->setFixedHeight(w_title_bar->height());

    auto *title_wrapper_layout = new QHBoxLayout(title_wrapper);
    title_wrapper_layout->setContentsMargins(resize_region, resize_region, resize_region, resize_region);
    title_wrapper_layout->setAlignment(Qt::AlignVCenter);
    title_wrapper_layout->addWidget(w_title_bar);
    
    main_layout->addWidget(title_wrapper, Qt::AlignTop);
    
    connect(this, &QWidget::windowTitleChanged, [this](const QString &title) {
      w_title_bar->get_ui().title->setText(title);
    });
  }
  const title_bar* widget::get_title_bar() const noexcept {
    return w_title_bar;
  }
  void widget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      current_edge = get_edge(event->globalPos() - geometry().topLeft());
      set_cursor(current_edge);
    }
    event->accept();
  }
  void widget::mouseMoveEvent(QMouseEvent *event) {
    if (current_edge == edge::none) {
      set_cursor(get_edge(event->globalPos() - geometry().topLeft()));
    } else if (event->buttons() & Qt::LeftButton) {

      QPoint event_cursor = event->globalPos();
      QPoint top_left = geometry().topLeft(); 

      auto check_new_size = [this](int new_width, int new_height) {
        return new_width >= minimumWidth() && new_width <= maximumWidth()  
               && new_height >= minimumHeight() && new_height <= maximumHeight();
      };

      switch (current_edge) {
        case edge::bottom | edge::left: {
          int new_width = width() + (top_left.x() - event_cursor.x());
          int new_height = event_cursor.y() - top_left.y();
          if (check_new_size(new_width, new_height)) {
            setGeometry(event_cursor.x(), top_left.y(), new_width, new_height);
          }
          break;
        }
        case edge::bottom | edge::right: {
          resize(event_cursor.x() - top_left.x(), event_cursor.y() - top_left.y());
          break;
        }
        case edge::top | edge::left: {
          int new_width = width() + (top_left.x() - event_cursor.x());
          int new_height = height() + (top_left.y() - event_cursor.y());
          if (check_new_size(new_width, new_height)) {
            setGeometry(event_cursor.x(), event_cursor.y(), new_width, new_height);
          }
          break;
        }
        case edge::top | edge::right: {
          int new_width = event_cursor.x() - top_left.x();
          int new_height = height() + (top_left.y() - event_cursor.y());
          if (check_new_size(new_width, new_height)) {
            setGeometry(top_left.x(), event_cursor.y(), new_width, new_height);
          }
          break;
        }
        case edge::left: {
          int new_width = width() + (top_left.x() - event_cursor.x());
          if (check_new_size(new_width, height())) {
            setGeometry(event_cursor.x(), top_left.y(), new_width, height());
          }
          break;
        }
        case edge::right: {
          resize(event_cursor.x() - top_left.x(), height());
          break;
        }
        case edge::top: {
          int new_height = height() + (top_left.y() - event_cursor.y());
          if (check_new_size(width(), new_height)) {
            setGeometry(top_left.x(), event_cursor.y(), width(), new_height);
          }
          break;
        }
        case edge::bottom: {
          resize(width(), event_cursor.y() - top_left.y());
          break;
        }
      }
    }
    event->accept();
  }
  void widget::mouseReleaseEvent(QMouseEvent *event) {
    current_edge = edge::none;
    event->accept();
  }
  void widget::paintEvent(QPaintEvent *event) {
    QStyleOption option;
    option.init(this);
    QPainter paint(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &paint, this);

    QWidget::paintEvent(event);
  }
  title_bar::title_bar(QWidget *parent) noexcept : QWidget(parent), parent(parent) {
    setMouseTracking(true);

    ui.title = new QLabel("QFrameLess");

    ui.close_button = new QPushButton;
    ui.minimize_button = new QPushButton;
    ui.maximize_button = new QPushButton;

    ui.close_button->setProperty("class", "close_button");
    ui.close_button->setProperty("title_button", true);
    ui.minimize_button->setProperty("class", "minimize_button");
    ui.minimize_button->setProperty("title_button", true);
    ui.maximize_button->setProperty("class", "maximize_button");
    ui.maximize_button->setProperty("title_button", true);

    QPixmap pix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    ui.close_button->setIcon(pix);

    pix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
    ui.minimize_button->setIcon(pix);

    pix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
    ui.maximize_button->setIcon(pix);

    auto *button_layout = new QHBoxLayout;
    button_layout->setSpacing(0);
    button_layout->setAlignment(Qt::AlignCenter);
    button_layout->addWidget(ui.minimize_button);
    button_layout->addWidget(ui.maximize_button);
    button_layout->addWidget(ui.close_button);

    main_layout = new QHBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    main_layout->addWidget(ui.title, Qt::AlignHCenter);
    main_layout->addLayout(button_layout);
    
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->addLayout(main_layout);

    setFixedHeight(title_height);

    connect(ui.close_button, &QPushButton::clicked, parent, &QWidget::close);
    connect(ui.minimize_button, &QPushButton::clicked, parent, &QWidget::showMinimized);
    connect(ui.maximize_button, &QPushButton::clicked, parent, &QWidget::showMaximized);
  }
  void title_bar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
      cursor = event->globalPos() - parent->geometry().topLeft();
    }
    event->accept();
  }
  void title_bar::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
      parent->move(event->globalPos() - cursor);
      setCursor(Qt::ClosedHandCursor);
    }
    event->accept();
  }
  void title_bar::mouseReleaseEvent(QMouseEvent *event) {
    setCursor(Qt::ArrowCursor);
    event->accept();
  }
  void title_bar::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
  }
  const title_bar::_ui& title_bar::get_ui() const noexcept {
    return ui;
  }
  int16_t widget::get_edge(const QPoint &point) {
    bool is_left = point.x() <= resize_region;
    bool is_right = point.x() >= width() - resize_region;
    bool is_top = point.y() <= resize_region;
    bool is_bottom = point.y() >= height() - resize_region;

    int16_t current_edge = edge::none;

    if (is_left) current_edge |= edge::left;
    if (is_right) current_edge |= edge::right;
    if (is_top) current_edge |= edge::top;
    if (is_bottom) current_edge |= edge::bottom;

    return current_edge;
  }
  void widget::set_cursor(int16_t edge_pos) {
    if (edge_pos & edge::left) {
      if (edge_pos & edge::bottom) setCursor(Qt::SizeBDiagCursor);
      else if (edge_pos & edge::top) setCursor(Qt::SizeFDiagCursor);
      else setCursor(Qt::SizeHorCursor);
    } else if (edge_pos & edge::right) {
      if (edge_pos & edge::bottom) setCursor(Qt::SizeFDiagCursor);
      else if (edge_pos & edge::top) setCursor(Qt::SizeBDiagCursor);
      else setCursor(Qt::SizeHorCursor);
    } else if (edge_pos & edge::top || edge_pos & edge::bottom) {
      setCursor(Qt::SizeVerCursor);
    } else {
      setCursor(Qt::ArrowCursor);
    }
  }
}