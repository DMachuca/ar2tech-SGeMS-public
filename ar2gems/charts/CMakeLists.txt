# Process all headers with Q_OBJECT
QT4_WRAP_CPP(charts_MOC
  categorical_histogram_creator.h
  categorical_histogram_proxy_model.h
  chart_base_display_controls.h
  chart_base.h
  chart_categorical_histogram.h
  chart_continuous_histogram.h
  chart_creator.h
  chart_dialog.h
  chart_display_control.h
  chart_distribution.h
  chart_matrix_widget.h
  chart_mdi_area.h
  chart_option_controls.h
  chart_scatter_plot.h
  chart_widget.h
  continuous_histogram_creator.h
  histogram_chart_model.h
  histogram_item_tree_view.h
  histogram_proxy_model.h
  histogram_style_delegate.h
  log_data_chart_creator.h
  log_data_chart.h
  region_weight_delegate.h
  scatter_plot_creator.h
  scatter_plot_item_tree_view.h
  scatter_plot_point_style_delegate.h
  scatter_plot_proxy_model.h
)

QT4_WRAP_UI(charts_FORMS
  chart_display_control.ui
)

add_library(ar2gems_charts
  categorical_histogram_creator.cpp
  categorical_histogram_model_item.cpp
  categorical_histogram_proxy_model.cpp
  chart_base.cpp
  chart_base_display_controls.cpp
  chart_categorical_histogram.cpp
  chart_continuous_histogram.cpp
  chart_dialog.cpp
  chart_display_control.cpp
  chart_distribution.cpp
  chart_matrix_widget.cpp
  chart_mdi_area.cpp
  chart_option_controls.cpp
  chart_scatter_plot.cpp
  chart_widget.cpp
  continuous_histogram_creator.cpp
  histogram_chart_model.cpp
  histogram_item_tree_view.cpp
  histogram_model_item.cpp
  histogram_proxy_model.cpp
  histogram_statistics.cpp
  histogram_style_delegate.cpp
  library_charts_init.cpp
  log_data_chart.cpp
  log_data_chart_creator.cpp
  multiple_charts_viewport.cpp
  region_weight_delegate.cpp
  scatter_plot_creator.cpp
  scatter_plot_item_tree_view.cpp
  scatter_plot_model_item.cpp
  scatter_plot_point_style_delegate.cpp
  scatter_plot_proxy_model.cpp
  categorical_histogram_creator.h
  categorical_histogram_model_item.h
  categorical_histogram_proxy_model.h
  chart_base.h
  chart_base_display_controls.h
  chart_categorical_histogram.h
  chart_continuous_histogram.h
  chart_dialog.h
  chart_display_control.h
  chart_distribution.h
  chart_matrix_widget.h
  chart_mdi_area.h
  chart_option_controls.h
  chart_scatter_plot.h
  chart_widget.h
  continuous_histogram_creator.h
  histogram_chart_model.h
  histogram_item_tree_view.h
  histogram_model_item.h
  histogram_proxy_model.h
  histogram_statistics.h
  histogram_style_delegate.h
  library_charts_init.h
  log_data_chart.h
  log_data_chart_creator.h
  multiple_charts_viewport.h
  region_weight_delegate.h
  scatter_plot_creator.h
  scatter_plot_item_tree_view.h
  scatter_plot_model_item.h
  scatter_plot_point_style_delegate.h
  scatter_plot_proxy_model.h
  ${charts_MOC}
  ${charts_FORMS}
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

target_link_libraries(ar2gems_charts
      ${QT_LIBRARIES}
      ${VTK_LIBRARIES}
      ar2gems_appli
      ar2gems_grid
      ar2gems_math
      ar2gems_widgets
      ar2gems_utils
)
set_target_properties( ar2gems_charts PROPERTIES OUTPUT_NAME_RELEASE ar2gems_charts )
set_target_properties( ar2gems_charts PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_charts_d )



if (WIN32)
  add_definitions(-DCHARTS_EXPORT)
endif()

         
INSTALL(TARGETS ar2gems_charts DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
