// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct ft8613_csot_5p7 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
};

static inline
struct ft8613_csot_5p7 *to_ft8613_csot_5p7(struct drm_panel *panel)
{
	return container_of(panel, struct ft8613_csot_5p7, panel);
}

static void ft8613_csot_5p7_reset(struct ft8613_csot_5p7 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(30);
}

static int ft8613_csot_5p7_on(struct ft8613_csot_5p7 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x87, 0x16, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x87, 0x16);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x81);
	mipi_dsi_dcs_write_seq(dsi, 0xf3,
			       0x40, 0x89, 0xc0, 0x40, 0x89, 0xc0, 0x40, 0x81,
			       0xc0, 0x40, 0x81, 0xc0);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);

	ret = mipi_dsi_dcs_set_column_address(dsi, 0x0000, 0x02cf);
	if (ret < 0) {
		dev_err(dev, "Failed to set column address: %d\n", ret);
		return ret;
	}

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);

	ret = mipi_dsi_dcs_set_page_address(dsi, 0x0000, 0x059f);
	if (ret < 0) {
		dev_err(dev, "Failed to set page address: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_display_brightness(dsi, 0x00ff);
	if (ret < 0) {
		dev_err(dev, "Failed to set display brightness: %d\n", ret);
		return ret;
	}

	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x24);
	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x11, 0x00);
	msleep(120);
	mipi_dsi_dcs_write_seq(dsi, 0x29, 0x00);
	usleep_range(2000, 3000);

	return 0;
}

static int ft8613_csot_5p7_off(struct ft8613_csot_5p7 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(20);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xf7, 0x5a, 0xa5, 0x87, 0x16);

	return 0;
}

static int ft8613_csot_5p7_prepare(struct drm_panel *panel)
{
	struct ft8613_csot_5p7 *ctx = to_ft8613_csot_5p7(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	ft8613_csot_5p7_reset(ctx);

	ret = ft8613_csot_5p7_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int ft8613_csot_5p7_unprepare(struct drm_panel *panel)
{
	struct ft8613_csot_5p7 *ctx = to_ft8613_csot_5p7(panel);


	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	return 0;
}

static int ft8613_csot_5p7_disable(struct drm_panel *panel)
{
	struct ft8613_csot_5p7 *ctx = to_ft8613_csot_5p7(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = ft8613_csot_5p7_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	return 0;
}

static const struct drm_display_mode ft8613_csot_5p7_mode = {
	.clock = (720 + 32 + 2 + 32) * (1440 + 16 + 2 + 16) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 32,
	.hsync_end = 720 + 32 + 2,
	.htotal = 720 + 32 + 2 + 32,
	.vdisplay = 1440,
	.vsync_start = 1440 + 16,
	.vsync_end = 1440 + 16 + 2,
	.vtotal = 1440 + 16 + 2 + 16,
	.width_mm = 65,
	.height_mm = 129,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int ft8613_csot_5p7_get_modes(struct drm_panel *panel,
				     struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &ft8613_csot_5p7_mode);
}

static const struct drm_panel_funcs ft8613_csot_5p7_panel_funcs = {
	.prepare = ft8613_csot_5p7_prepare,
	.unprepare = ft8613_csot_5p7_unprepare,
	.disable = ft8613_csot_5p7_disable,
	.get_modes = ft8613_csot_5p7_get_modes,
};

static int ft8613_csot_5p7_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct ft8613_csot_5p7 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vsn";
	ctx->supplies[1].supply = "vsp";
	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(ctx->supplies),
				      ctx->supplies);
	if (ret < 0)
		return dev_err_probe(dev, ret, "Failed to get regulators\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &ft8613_csot_5p7_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void ft8613_csot_5p7_remove(struct mipi_dsi_device *dsi)
{
	struct ft8613_csot_5p7 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id ft8613_csot_5p7_of_match[] = {
	{ .compatible = "xiaomi,rosy-ft8613-csot" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ft8613_csot_5p7_of_match);

static struct mipi_dsi_driver ft8613_csot_5p7_driver = {
	.probe = ft8613_csot_5p7_probe,
	.remove = ft8613_csot_5p7_remove,
	.driver = {
		.name = "panel-ft8613-csot-5p7",
		.of_match_table = ft8613_csot_5p7_of_match,
	},
};
module_mipi_dsi_driver(ft8613_csot_5p7_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for ft8613_csot_5p7_720p_video");
MODULE_LICENSE("GPL");
