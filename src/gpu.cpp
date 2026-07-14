/*============================================================================
Copyright (c) 2024 Raspberry Pi
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
============================================================================*/

#include <glibmm.h>
#include "gpu.hpp"

extern "C" {
    PanelWidget *create () { return new WidgetGPU; }
    void destroy (PanelWidget *w) { delete w; }

    const conf_table_t *config_params (void) { return conf_table; };
    const char *display_name (void) { return PLUGIN_TITLE; };
    const char *package_name (void) { return GETTEXT_PACKAGE; };
}

bool WidgetGPU::set_icon (void)
{
    gpu_update_display (gpu);
    return false;
}

void WidgetGPU::read_settings (void)
{
    gpu->show_percentage = show_percentage;
    if (!gdk_rgba_parse (&gpu->foreground_colour, ((std::string) foreground_colour).c_str()))
        gdk_rgba_parse (&gpu->foreground_colour, "dark gray");
    if (!gdk_rgba_parse (&gpu->background_colour, ((std::string) background_colour).c_str()))
        gdk_rgba_parse (&gpu->background_colour, "light gray");
}

void WidgetGPU::settings_changed_cb (void)
{
    read_settings ();
    gpu_update_display (gpu);
}

void WidgetGPU::init (Gtk::HBox *container)
{
    /* Create the button */
    plugin = std::make_unique <Gtk::Button> ();
    plugin->set_name (PLUGIN_NAME);
    container->pack_start (*plugin, false, false);

    /* Setup structure */
    gpu = g_new0 (GPUPlugin, 1);
    gpu->plugin = (GtkWidget *)((*plugin).gobj());
    icon_timer = Glib::signal_idle().connect (sigc::mem_fun (*this, &WidgetGPU::set_icon));

    /* Initialise the plugin */
    read_settings ();
    gpu_init (gpu);

    /* Setup callbacks */
    show_percentage.set_callback (sigc::mem_fun (*this, &WidgetGPU::settings_changed_cb));
    foreground_colour.set_callback (sigc::mem_fun (*this, &WidgetGPU::settings_changed_cb));
    background_colour.set_callback (sigc::mem_fun (*this, &WidgetGPU::settings_changed_cb));
}

WidgetGPU::~WidgetGPU()
{
    icon_timer.disconnect ();
    gpu_destructor (gpu);
}

/* End of file */
/*----------------------------------------------------------------------------*/
