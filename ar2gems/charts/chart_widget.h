/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH) 
** version of the open source software sgems.  It is a derivative work by 
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original 
** version of the software (see notice below) and now sublicensed such that it 
** cannot be distributed or modified without the explicit and written permission 
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this 
** file/software.
**
** This file cannot be modified or distributed without the explicit and written 
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at 
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/



#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

#include <charts/common.h>

#include <charts/chart_display_control.h>

#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>

#include <QColor>
#include <QToolBox>
#include <QTabWidget>
#include <QModelIndexList>
#include <QDropEvent>
#include <QSize>


class CHARTS_DECL Chart_widget : public QVTKWidget
{
  Q_OBJECT

public:
    Chart_widget(QWidget *parent=0);
    virtual ~Chart_widget();

    vtkChartXY* chart() {return chart_.GetPointer();}

    void set_controler(Chart_display_control* controler);

    
    void save_figure(QString& filename,QSize plot_size = QSize() );

public slots :

  void save_figure();
  
  void reset_axis();
  void set_x_axis_label(const QString& text);
  void set_y_axis_label(const QString& text);
  void set_title(const QString& text);
  void set_legend(bool);
  void set_grid(bool);
  void set_x_grid(bool);
  void set_y_grid(bool);

  void set_xaxis_min(double min);
  void set_xaxis_max(double max);
  void set_xaxis_precision(int digits);
  void set_xaxis_nticks(int nticks);
  void set_xaxis_logscale(bool on);
  void set_xaxis_autoscale();

  void set_yaxis_min(double min);
  void set_yaxis_max(double max);
  void set_yaxis_precision(int digits);
  void set_yaxis_nticks(int nticks);
  void set_yaxis_logscale(bool on);
  void set_yaxis_autoscale();

  void set_x_axis_font_size(int size);
  void set_y_axis_font_size(int size);
  void set_x_label_font_size(int size);
  void set_y_label_font_size(int size);
  void set_legend_font_size(int size);
  void set_title_font_size(int size);


protected:

  void update_chart_axis_display_control();

protected:

  Chart_display_control* chart_control_;

	vtkSmartPointer<vtkChartXY> chart_;
	vtkSmartPointer<vtkContextView> context_view_;

    
};


#endif // CHART_BASE_DISPLAY_CONTROLS_H
