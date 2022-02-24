// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 StarFive Technology Co., Ltd.
 */
#ifndef __JPU_STARFIVE_H__
#define __JPU_STARFIVE_H__

#include <linux/io.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/reset-controller.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/clk-provider.h>

struct starfive_jpu_data {
	struct device *dev;

	struct clk *clk_jpeg_axi;
	struct clk *clk_jpeg_cclk;
	struct clk *clk_jpeg_apb;

    struct reset_control *rst_jpeg_axi;
    struct reset_control *rst_jpeg_cclk;
    struct reset_control *rst_jpeg_apb;

};

static struct starfive_jpu_data *sf_jpu = NULL;

void starfive_jpu_rst_status(void)
{
	int rst_state;
	rst_state = reset_control_status(sf_jpu->rst_jpeg_axi);
	printk("jpeg_axi status_rst:%d", rst_state);
	//dev_dbg(sf_jpu->dev, "status_rst :%d", rst_state);

	rst_state = reset_control_status(sf_jpu->rst_jpeg_cclk);
	printk("jpeg_cclk status_rst:%d", rst_state);
	//dev_dbg(sf_jpu->dev, "status_rst :%d", rst_state);

	rst_state = reset_control_status(sf_jpu->rst_jpeg_apb);
	printk("jpeg_apb status_rst:%d", rst_state);
	//dev_dbg(sf_jpu->dev, "status_rst :%d", rst_state);
	
	return;
}

void starfive_jpu_clk_status(void)
{
	int clk_state;
	clk_state = __clk_is_enabled(sf_jpu->clk_jpeg_axi);
	printk("jpeg_axi status_clk:%d", clk_state);
	//dev_dbg(sf_venc->dev, "status_clk :%d", clk_state);

	clk_state = __clk_is_enabled(sf_jpu->clk_jpeg_cclk);
	printk("jpeg_cclk status_clk:%d", clk_state);
	//dev_dbg(sf_venc->dev, "status_clk :%d", clk_state);

	clk_state = __clk_is_enabled(sf_jpu->clk_jpeg_apb);
	printk("jpeg_apb status_clk:%d", clk_state);
	//dev_dbg(sf_venc->dev, "status_clk :%d", clk_state);

	return;
}

void starfive_jpu_rst_exit(void)
{	
	int ret = 0;
	//ret = reset_control_assert(sf_jpu->rst_jpeg_axi);
	if (ret) 
		dev_err(sf_jpu->dev, "jpeg_axi reset assert failed\n");
	
    ret = reset_control_assert(sf_jpu->rst_jpeg_cclk);
	if (ret) 
		dev_err(sf_jpu->dev, "jpeg_cclk reset assert failed\n");
	
    ret = reset_control_assert(sf_jpu->rst_jpeg_apb);
	if (ret) 
		dev_err(sf_jpu->dev, "jpeg_apb reset assert failed\n");

	return;
}

void starfive_jpu_clk_exit(void)
{
    clk_disable_unprepare(sf_jpu->clk_jpeg_apb);
    clk_disable_unprepare(sf_jpu->clk_jpeg_cclk);
	clk_disable_unprepare(sf_jpu->clk_jpeg_axi);
	return;
}

static int starfive_jpu_clk_init(void)
{
	int ret = 0;

	ret = clk_prepare_enable(sf_jpu->clk_jpeg_axi);
	if (ret) {
		dev_err(sf_jpu->dev, "jpu axi clock enable failed\n");
		return ret;
	}

	ret = clk_prepare_enable(sf_jpu->clk_jpeg_cclk);
	if (ret) {
		dev_err(sf_jpu->dev, "jpu cclk clock enable failed\n");
		goto clk_cclk_failed;
	}

	ret = clk_prepare_enable(sf_jpu->clk_jpeg_apb);
	if (ret) {
		dev_err(sf_jpu->dev, "jpu apb clock enable failed\n");
		goto clk_apb_failed;
	}

	return 0;

clk_apb_failed:
	clk_disable_unprepare(sf_jpu->clk_jpeg_cclk);

clk_cclk_failed:
	clk_disable_unprepare(sf_jpu->clk_jpeg_axi);

	return ret;
}

static int starfive_jpu_get_clk(void)
{
	int ret = 0;

	sf_jpu->clk_jpeg_axi = devm_clk_get(sf_jpu->dev, "jpeg_axi");
	if (IS_ERR(sf_jpu->clk_jpeg_axi)) {
		dev_err(sf_jpu->dev, "failed to get jpu axi clock\n");
		return PTR_ERR(sf_jpu->clk_jpeg_axi);
	}

	sf_jpu->clk_jpeg_cclk = devm_clk_get(sf_jpu->dev, "jpeg_cclk");
	if (IS_ERR(sf_jpu->clk_jpeg_cclk)) {
		dev_err(sf_jpu->dev, "failed to get jpu cclk clock\n");
		ret = PTR_ERR(sf_jpu->clk_jpeg_cclk);
		goto clk_cclk_failed;
	}

	sf_jpu->clk_jpeg_apb = devm_clk_get(sf_jpu->dev, "jpeg_apb");
	if (IS_ERR(sf_jpu->clk_jpeg_apb)) {
		dev_err(sf_jpu->dev, "failed to get jpu apb clock\n");
		ret = PTR_ERR(sf_jpu->clk_jpeg_apb);
		goto clk_apb_failed;
	}
	
	return 0;

clk_apb_failed:
	devm_clk_put(sf_jpu->dev, sf_jpu->clk_jpeg_cclk);

clk_cclk_failed:
	devm_clk_put(sf_jpu->dev, sf_jpu->clk_jpeg_axi);

	return ret;
}

static int starfive_jpu_reset_init(void)
{
    int ret = 0;

    ret = reset_control_deassert(sf_jpu->rst_jpeg_axi);
    if (ret) {
		dev_err(sf_jpu->dev, "jpu axi reset failed\n");
        return ret;
	}

    ret = reset_control_deassert(sf_jpu->rst_jpeg_cclk);
    if (ret) {
		dev_err(sf_jpu->dev, "jpu cclk reset failed\n");
        goto rst_cclk_failed;
	}
    ret = reset_control_deassert(sf_jpu->rst_jpeg_apb);
    if (ret) {
		dev_err(sf_jpu->dev, "jpu apb reset failed\n");
        goto rst_apb_failed;
	}

    return 0;

rst_apb_failed:
	reset_control_assert(sf_jpu->rst_jpeg_cclk);

rst_cclk_failed:
	reset_control_assert(sf_jpu->rst_jpeg_axi);

	return ret;
}

static int starfive_jpu_get_resets(void)
{
	
    int ret = 0;
	
	sf_jpu->rst_jpeg_axi = devm_reset_control_get_exclusive(sf_jpu->dev, "jpeg_axi");
	if (IS_ERR(sf_jpu->rst_jpeg_axi)) {
		dev_err(sf_jpu->dev, "failed to get jpeg_axi resets\n");
		return PTR_ERR(sf_jpu->rst_jpeg_axi);
	}

	sf_jpu->rst_jpeg_cclk = devm_reset_control_get_exclusive(sf_jpu->dev, "jpeg_cclk");
	if (IS_ERR(sf_jpu->rst_jpeg_cclk)) {
		dev_err(sf_jpu->dev, "failed to get jpeg_cclk resets\n");
		ret = PTR_ERR(sf_jpu->rst_jpeg_cclk);
		goto rst_cclk_failed;
	}

	sf_jpu->rst_jpeg_apb = devm_reset_control_get_exclusive(sf_jpu->dev, "jpeg_apb");
	if (IS_ERR(sf_jpu->rst_jpeg_apb)) {
		dev_err(sf_jpu->dev, "failed to get jpeg_apb resets\n");
		ret = PTR_ERR(sf_jpu->rst_jpeg_apb);
		goto rst_apb_failed;
	}
    
	return 0;

rst_apb_failed:
	reset_control_put(sf_jpu->rst_jpeg_cclk);

rst_cclk_failed:
	reset_control_put(sf_jpu->rst_jpeg_axi);

	return ret;
}

int starfive_jpu_data_init(struct device *dev)
{
	int ret = 0;

	if (sf_jpu == NULL){
		sf_jpu = devm_kzalloc(dev, sizeof(*sf_jpu), GFP_KERNEL);
		if (!sf_jpu)
			return -ENOMEM;
		sf_jpu->dev = dev;

		ret = starfive_jpu_get_clk();
		if (ret) {
			dev_err(sf_jpu->dev, "failed to get jpu clock\n");
			return ret;
		}

		ret = starfive_jpu_get_resets();
		if (ret) {
			dev_err(sf_jpu->dev, "failed to get jpu resets\n");
			return ret;
		}		
	}

	return 0;
}

int starfive_jpu_clk_enable(struct device *dev)
{
    int ret = 0;

	ret = starfive_jpu_data_init(dev);
	if (ret)
		return ret;

	ret = starfive_jpu_clk_init();
	if (ret) {
		dev_err(sf_jpu->dev, "failed to enable jpu clock\n");
		return ret;
	}

	return 0;
}

int starfive_jpu_clk_disable(struct device *dev)
{
    int ret = 0;

	ret = starfive_jpu_data_init(dev);
	if (ret)
		return ret;
    
	starfive_jpu_clk_exit();
	starfive_jpu_clk_status();

	return 0;
	
}

int starfive_jpu_rst_deassert(struct device *dev)
{
    int ret = 0;

	ret = starfive_jpu_data_init(dev);
	if (ret)
		return ret;

	ret = starfive_jpu_reset_init();
	if (ret) {
		dev_err(sf_jpu->dev, "failed to deassert jpu resets\n");
		return ret;
	}

	return 0;
}

int starfive_jpu_rst_assert(struct device *dev)
{
    int ret = 0;

	ret = starfive_jpu_data_init(dev);
	if (ret)
		return ret;
    
	starfive_jpu_rst_exit();
	starfive_jpu_rst_status();

	return 0;
	
}


int starfive_jpu_clk_rst_init(struct platform_device *pdev)
{
    int ret = 0;

	ret = starfive_jpu_data_init(&pdev->dev);
	if (ret)
		return ret;
 
    //clk
	ret = starfive_jpu_clk_init();
	if (ret) {
		dev_err(sf_jpu->dev, "failed to enable jpu clock\n");
		return ret;
	}
	starfive_jpu_clk_status(); 

    //resets
	starfive_jpu_rst_exit();
	mdelay(1);
	ret = starfive_jpu_reset_init();
	if (ret) {
		dev_err(sf_jpu->dev, "failed to set jpu reset\n");
		goto init_failed;
	}

	starfive_jpu_rst_status();
	printk(">>>>>starfive_jpu_clk_rst_init finish");
    return 0;

init_failed:
    starfive_jpu_clk_exit();
    return ret;

}

#endif

