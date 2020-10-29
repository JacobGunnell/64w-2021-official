#include "GUI.h"

int autonNum = 0;
Position position = {Position::RED, Position::LEFT};

static lv_res_t side_switch_action(lv_obj_t *);
static lv_res_t pos_switch_action(lv_obj_t *);
static lv_res_t autonlist_action(lv_obj_t *);

static lv_res_t gui_mbox_destroy(lv_obj_t *, const char *);


static lv_obj_t *preloader;
static lv_obj_t *mbox;
static lv_obj_t *autonList;
static lv_obj_t *descLabel;


void gui_loading_start()
{
  preloader = lv_preload_create(lv_scr_act(), NULL);
  lv_obj_set_size(preloader, 100, 100);
  lv_obj_align(preloader, NULL, LV_ALIGN_CENTER, 0, 0);
}

void gui_loading_stop()
{
  lv_obj_del(preloader);
}

void gui_error(std::string msg)
{
  mbox = lv_mbox_create(lv_scr_act(), NULL);
  lv_mbox_set_text(mbox, msg.c_str());
  static const char *btn_str[] = {"Close", ""};
  lv_mbox_add_btns(mbox, btn_str, NULL);
  lv_mbox_set_action(mbox, gui_mbox_destroy);
}

void gui_main()
{
	lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), NULL);


	lv_obj_t *autonSetup = lv_tabview_add_tab(tabview, "Match Setup");

	lv_obj_t *title4 = lv_label_create(autonSetup, NULL);
	lv_label_set_text(title4, "Select Field Position");
	lv_obj_align(title4, NULL, LV_ALIGN_IN_TOP_LEFT, 30, 0);

	static lv_style_t side_bg_style;
	static lv_style_t side_indic_style;
	static lv_style_t side_knob_on_style;
	static lv_style_t side_knob_off_style;
	lv_style_copy(&side_bg_style, &lv_style_pretty);
	side_bg_style.body.radius = LV_RADIUS_CIRCLE;
	side_bg_style.body.main_color = LV_COLOR_HEX(0xef9f9f);
	side_bg_style.body.grad_color = LV_COLOR_HEX(0xef9f9f);
	lv_style_copy(&side_indic_style, &lv_style_pretty_color);
	side_indic_style.body.radius = LV_RADIUS_CIRCLE;
	side_indic_style.body.main_color = LV_COLOR_HEX(0x9fc8ef);
	side_indic_style.body.grad_color = LV_COLOR_HEX(0x9fc8ef);
	side_indic_style.body.padding.hor = 0;
	side_indic_style.body.padding.ver = 0;
	lv_style_copy(&side_knob_off_style, &lv_style_pretty_color);
	side_knob_off_style.body.radius = LV_RADIUS_CIRCLE;
	side_knob_off_style.body.shadow.width = 4;
	side_knob_off_style.body.shadow.type = LV_SHADOW_BOTTOM;
	side_knob_off_style.body.main_color = LV_COLOR_RED;
	side_knob_off_style.body.grad_color = LV_COLOR_RED;
	lv_style_copy(&side_knob_on_style, &lv_style_pretty_color);
	side_knob_on_style.body.radius = LV_RADIUS_CIRCLE;
	side_knob_on_style.body.shadow.width = 4;
	side_knob_on_style.body.shadow.type = LV_SHADOW_BOTTOM;
	side_knob_on_style.body.main_color = LV_COLOR_BLUE;
	side_knob_on_style.body.grad_color = LV_COLOR_BLUE;

	lv_obj_t *sw1 = lv_sw_create(autonSetup, NULL);
	lv_sw_set_style(sw1, LV_SW_STYLE_BG, &side_bg_style);
	lv_sw_set_style(sw1, LV_SW_STYLE_INDIC, &side_indic_style);
	lv_sw_set_style(sw1, LV_SW_STYLE_KNOB_ON, &side_knob_on_style);
	lv_sw_set_style(sw1, LV_SW_STYLE_KNOB_OFF, &side_knob_off_style);
	lv_obj_align(sw1, title4, LV_ALIGN_CENTER, -50, 45);
	lv_sw_set_action(sw1, side_switch_action);

	static lv_style_t pos_bg_style;
	static lv_style_t pos_indic_style;
	static lv_style_t pos_knob_on_style;
	static lv_style_t pos_knob_off_style;
	lv_style_copy(&pos_bg_style, &lv_style_pretty);
	pos_bg_style.body.radius = LV_RADIUS_CIRCLE;
	lv_style_copy(&pos_indic_style, &lv_style_pretty);
	pos_indic_style.body.radius = LV_RADIUS_CIRCLE;
	pos_indic_style.body.padding.hor = 0;
	pos_indic_style.body.padding.ver = 0;
	lv_style_copy(&pos_knob_off_style, &lv_style_pretty);
	pos_knob_off_style.body.radius = LV_RADIUS_CIRCLE;
	pos_knob_off_style.body.shadow.width = 4;
	pos_knob_off_style.body.shadow.type = LV_SHADOW_BOTTOM;
	lv_style_copy(&pos_knob_on_style, &lv_style_pretty);
	pos_knob_on_style.body.radius = LV_RADIUS_CIRCLE;
	pos_knob_on_style.body.shadow.width = 4;
	pos_knob_on_style.body.shadow.type = LV_SHADOW_BOTTOM;

	lv_obj_t *sw2 = lv_sw_create(autonSetup, NULL);
	lv_sw_set_style(sw2, LV_SW_STYLE_BG, &pos_bg_style);
	lv_sw_set_style(sw2, LV_SW_STYLE_INDIC, &pos_indic_style);
	lv_sw_set_style(sw2, LV_SW_STYLE_KNOB_ON, &pos_knob_on_style);
	lv_sw_set_style(sw2, LV_SW_STYLE_KNOB_OFF, &pos_knob_off_style);
	lv_obj_align(sw2, sw1, LV_ALIGN_CENTER, 0, 50);
	lv_sw_set_action(sw2, pos_switch_action);

	lv_obj_t *title3 = lv_label_create(autonSetup, NULL);
	lv_label_set_text(title3, "Select an Auton.");
	lv_obj_align(title3, NULL, LV_ALIGN_IN_TOP_MID, 100, 20);

	static lv_style_t style_bg;
	lv_style_copy(&style_bg, &lv_style_pretty);
	style_bg.body.shadow.width = 4;
	style_bg.text.color = LV_COLOR_MAKE(0x10, 0x20, 0x50);

  descLabel = lv_label_create(autonSetup, NULL);
  lv_obj_align(descLabel, title3, LV_ALIGN_CENTER, 100, 60);
  lv_label_set_long_mode(descLabel, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(descLabel, 240);
  lv_label_set_text(descLabel, AutonBase::getAllObjs()[autonNum]->getDescription().c_str());

	autonList = lv_ddlist_create(autonSetup, NULL);
	lv_obj_set_free_num(autonList, 2);
	lv_obj_set_style(autonList, &style_bg);
	lv_ddlist_set_options(autonList, AutonBase::getAllListings().c_str());
	lv_obj_align(autonList, title3, LV_ALIGN_CENTER, 0, 30);
	lv_ddlist_set_action(autonList, autonlist_action);


  lv_obj_t *tuning = lv_tabview_add_tab(tabview, "Tuning");

  // TODO
}


static lv_res_t autonlist_action(lv_obj_t *list) // TODO: gray out impossible options with a specific Position
{
  autonNum = lv_ddlist_get_selected(list);
  lv_label_set_text(descLabel, AutonBase::getAllObjs()[autonNum]->getDescription().c_str());

  return LV_RES_OK;
}

static lv_res_t pos_switch_action(lv_obj_t *sw)
{
  if(position.pos == Position::LEFT)
  {
    position.pos = Position::RIGHT;
    lv_sw_on(sw);
  }
  else if(position.pos == Position::RIGHT)
  {
    position.pos = Position::LEFT;
    lv_sw_off(sw);
  }

  //autonlist_update(); // TODO

  return LV_RES_OK;
}

static lv_res_t side_switch_action(lv_obj_t *sw)
{
  if(position.side == Position::RED)
  {
    position.side = Position::BLUE;
    lv_sw_on(sw);
  }
  else if(position.side == Position::BLUE)
  {
    position.side = Position::RED;
    lv_sw_off(sw);
  }

  //autonlist_update(); // TODO

  return LV_RES_OK;
}

static lv_res_t gui_mbox_destroy(lv_obj_t *obj, const char *c)
{
  lv_obj_del(lv_mbox_get_from_btn(obj));
  return LV_RES_OK;
}
