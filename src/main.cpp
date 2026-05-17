#include "Geode/ui/NineSlice.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <Geode/utils/ColorProvider.hpp>
#include <Geode/utils/general.hpp>

// #include <geode.custom-keybinds/include/Keybinds.hpp>  // Geode v4.x Legacy code
#include <Geode/loader/GameEvent.hpp>

#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/SettingV3.hpp>


namespace progtracklabel {
	class LabelLayer : public CCLayer {
	public:
		static LabelLayer* create() {
			LabelLayer* layer = new LabelLayer();
			if (layer->init()) {
				layer->autorelease();
				return layer;
			}
			CC_SAFE_DELETE(layer);
			return nullptr;
		}

		bool init() override {
			if (!CCLayer::init()) { return false; }
			return true;
		}
	};
	class PLTLayer : public Popup {  // Popup<GJGameLevel* const&> {  // Geode v4.x Legacy code
	protected:
		//// Geode v4.x Legacy code ->
		// bool setup(GJGameLevel* const& level) override {
		// 	return true;
		// }
		bool init() {
			if (!Popup::init(368.f, 280.f, "GJ_square05.png", {0.f, 0.f, 80.f, 80.f}))
				return false;

        	return true;
		}
	public:
		static PLTLayer* create(GJGameLevel* const& Level) {
			PLTLayer* self = new PLTLayer();
			// if (self && self->initAnchored(368, 280, Level, "GJ_square05.png", {0.f, 0.f, 80.f, 80.f})) {  // Geode v4.x Legacy code
			if (self && self->init()) {
				self->autorelease();
				return self;
			}
			CC_SAFE_DELETE(self);
			return nullptr;
		}
		void onClose(CCObject*) {
			this->removeFromParentAndCleanup(true);
		}
	};
	namespace reset_layer {
		void set_position(progtracklabel::LabelLayer* layer_) {
			layer_->setPositionX(Mod::get()->getSettingValue<int>("position-x"));
			layer_->setPositionY(Mod::get()->getSettingValue<int>("position-y"));
			layer_->setContentSize(CCSizeZero);
			layer_->ignoreAnchorPointForPosition(true);
		}

		void set_label_text(CCLabelBMFont* label_main) {
			// CCLabelBMFont* label_main = (CCLabelBMFont*)layer_->getChildByID("plt-text-label");
			std::string label_text = Mod::get()->getSettingValue<std::string>("label-main");
			std::string newline_replacing_string = Mod::get()->getSettingValue<std::string>("newline-replacement");
			std::string counter_replacing_string_format = Mod::get()->getSettingValue<std::string>("counter-replacement");
			int n = 0;
			while ((n = label_text.find(newline_replacing_string, n)) != std::string::npos) {
				label_text.replace(n, newline_replacing_string.size(),"\n");
				n += newline_replacing_string.size();
			}
			std::string counter_replacing_string_i; int position, counter_value_i;
			for (int i = 1; i < 10; i++) {
				counter_replacing_string_i = counter_replacing_string_format;
				counter_replacing_string_i.replace(counter_replacing_string_i.find("{}"), 2, fmt::format("{}", i));
				if ((position = label_text.find(counter_replacing_string_i)) != std::string::npos) {
					counter_value_i = Mod::get()->getSavedValue<int>(fmt::format("counter-{}", i));
					label_text.replace(position, counter_replacing_string_i.size(), fmt::format("{}", counter_value_i));
				}
			}
			label_main->setString(label_text.c_str());
		}

		void set_label_alignment(CCLabelBMFont* label_main) {
			std::string alignment = Mod::get()->getSettingValue<std::string>("text-alignment");
			label_main->setPosition(CCPointZero);
			if      (alignment == "Left")   { label_main->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);   label_main->setAnchorPoint(CCPointMake(0.0f,0.5f)); }
			else if (alignment == "Center") { label_main->setAlignment(CCTextAlignment::kCCTextAlignmentCenter); label_main->setAnchorPoint(CCPointMake(0.5f,0.5f)); }
			else if (alignment == "Right")  { label_main->setAlignment(CCTextAlignment::kCCTextAlignmentRight);  label_main->setAnchorPoint(CCPointMake(1.0f,0.5f)); }
		}

		void set_label_color(CCLabelBMFont* label_main) {
			label_main->setColor(Mod::get()->getSettingValue<ccColor3B>("color"));
		}

		void set_label_scale(CCLabelBMFont* label_main) {
			label_main->setScale(Mod::get()->getSettingValue<float>("size"));
		}

		void set_label_opacity(CCLabelBMFont* label_main) {
			label_main->setOpacity(Mod::get()->getSettingValue<int>("opacity"));
		}

		void set_font_label(progtracklabel::LabelLayer* layer_) {
			std::string font_name;
			int font_num = Mod::get()->getSettingValue<int>("font");
			if      (font_num == 0)                   { font_name = "bigFont.fnt"; }
			else if (font_num == -1)                  { font_name = "chatFont.fnt"; }
			else if (font_num == -2)                  { font_name = "goldFont.fnt"; }
			else if (0 < font_num && font_num < 10)   { font_name = fmt::format("gjFont0{}.fnt", font_num); }
			else if (10 <= font_num && font_num < 60) { font_name = fmt::format("gjFont{}.fnt",  font_num); }
			CCLabelBMFont* label_main = CCLabelBMFont::create("", font_name.c_str());
			CCNode* old_label_ = layer_->getChildByID("plt-text-label");
			if (old_label_ != nullptr) layer_->removeChild(old_label_);
			progtracklabel::reset_layer::set_label_text(label_main);
			progtracklabel::reset_layer::set_label_alignment(label_main);
			progtracklabel::reset_layer::set_label_color(label_main);
			progtracklabel::reset_layer::set_label_scale(label_main);
			progtracklabel::reset_layer::set_label_opacity(label_main);
			label_main->setID("plt-text-label");
			layer_->addChild(label_main);
		}

		void external_reset_text() {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr) {
				auto label_main = (CCLabelBMFont*) layer_->getChildByID("plt-text-label");
				if (label_main != nullptr)
					progtracklabel::reset_layer::set_label_text(label_main);
			}
		}
	};
};

// using namespace keybinds;

$execute {
	if (true) {  // Register keybinds
		listenForKeybindSettingPresses("increase-counter-1", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
			if (down && !repeat) {
				// do something
			}
		});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-1"_spr,
	// 		"Increase counter 1", "",
	// 		{ Keybind::create(KEY_One, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-2"_spr,
	// 		"Increase counter 2", "",
	// 		{ Keybind::create(KEY_Two, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-3"_spr,
	// 		"Increase counter 3", "",
	// 		{ Keybind::create(KEY_Three, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-4"_spr,
	// 		"Increase counter 4", "",
	// 		{ Keybind::create(KEY_Four, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-5"_spr,
	// 		"Increase counter 5", "",
	// 		{ Keybind::create(KEY_Five, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-6"_spr,
	// 		"Increase Counter 6", "",
	// 		{ Keybind::create(KEY_Six, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-7"_spr,
	// 		"Increase Counter 7", "",
	// 		{ Keybind::create(KEY_Seven, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-8"_spr,
	// 		"Increase Counter 8", "",
	// 		{ Keybind::create(KEY_Eight, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	// 	BindManager::get()->registerBindable({
	// 		"increase-counter-9"_spr,
	// 		"Increase Counter 9", "",
	// 		{ Keybind::create(KEY_Nine, Modifier::None) },
	// 		"Play/Progress Track Label/Counters"
	// 	});
	}
	
	if (true) {  // Enable Setting Change tracking
		listenForSettingChanges<int>("position-x", [](int) {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr)
				progtracklabel::reset_layer::set_position(layer_);
		});
		listenForSettingChanges<int>("position-y", [](int) {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr)
				progtracklabel::reset_layer::set_position(layer_);
		});
		listenForSettingChanges<float>("size", [](float) {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr) {
				auto label_main = (CCLabelBMFont*) layer_->getChildByID("plt-text-label");
				if (label_main != nullptr)
					progtracklabel::reset_layer::set_label_scale(label_main);
			}
		});
		listenForSettingChanges<int>("opacity", [](int) {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr) {
				auto label_main = (CCLabelBMFont*) layer_->getChildByID("plt-text-label");
				if (label_main != nullptr)
					progtracklabel::reset_layer::set_label_opacity(label_main);
			}
		});
		listenForSettingChanges<std::string>("text-alignment", [](std::string) {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr) {
				auto label_main = (CCLabelBMFont*) layer_->getChildByID("plt-text-label");
				if (label_main != nullptr)
					progtracklabel::reset_layer::set_label_alignment(label_main);
			}
		});
		listenForSettingChanges<ccColor3B>("color", [](ccColor3B) {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr)
				progtracklabel::reset_layer::set_font_label(layer_);
		});
		listenForSettingChanges<int>("font", [](int) {
			auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
			if (layer_ != nullptr)
				progtracklabel::reset_layer::set_font_label(layer_);
		});
		listenForSettingChanges<std::string>("label-main", [](std::string) {
			progtracklabel::reset_layer::external_reset_text();
		});
		listenForSettingChanges<std::string>("newline-replacement", [](std::string) {
			progtracklabel::reset_layer::external_reset_text();
		});
		listenForSettingChanges<std::string>("counter-replacement", [](std::string) {
			progtracklabel::reset_layer::external_reset_text();
		});
	}

	if (true) {  // Set mod saved values()
		
	}
};

class $modify(UILayerModified, UILayer) {
	void register_keybinds() {
		for (int i = 1; i < 10; i++)
			this->addEventListener(
				KeybindSettingPressedEventV3(Mod::get(), fmt::format("increase-counter-{}", i)),
				[this, i] (Keybind const& keybind, bool down, bool repeat, double timestamp) {  // (InvokeBindEvent* event) {  // Geode v4.x Legacy code
				log::debug("Button {} // keybind {}, down {}, repeat {}, timestamp {}", i, keybind.toString(), down, repeat, timestamp);
					if (down && !repeat
					&& (Mod::get()->getSavedValue<bool>(fmt::format("counter-available-{}", i))
					    ||  !Mod::get()->getSavedValue<bool>(fmt::format("counter-sameattprevent-{}", i))
					    )) {
						PlayLayer* play_layer = PlayLayer::get();
						if (play_layer != nullptr)
							if (!play_layer->m_isPaused) {
								log::debug("Counter {} increased!", i);
								Mod::get()->setSavedValue<bool>(fmt::format("counter-available-{}", i), false);
								Mod::get()->setSavedValue<int>(fmt::format("counter-{}", i), Mod::get()->getSavedValue<int>(fmt::format("counter-{}", i)) + 1);
								progtracklabel::reset_layer::external_reset_text();
							}
					}
				return true;  // return ListenerResult::Propagate;  // Geode v4.x Legacy code
			});
	}

	bool init(GJBaseGameLayer* layer) {
		if (!UILayer::init(layer)) return false;

		if (!Mod::get()->getSettingValue<bool >("enable-mod")) return true;

		this->register_keybinds();
		// BindManager::get()->Bind

		progtracklabel::LabelLayer* layer_ = progtracklabel::LabelLayer::create();
		layer_->setID("progresstracklabel-labellayer");
		progtracklabel::reset_layer::set_position(layer_);
		progtracklabel::reset_layer::set_font_label(layer_);
		// progtracklabel::reset_layer::set_label_text(layer_);
		// progtracklabel::reset_layer::set_label_alignment(layer_);
		// progtracklabel::reset_layer::set_label_color(layer_);
		// progtracklabel::reset_layer::set_label_scale(layer_);
		// progtracklabel::reset_layer::set_label_opacity(layer_);

		this->addChild(layer_);
		return true;
	}
};

class $modify(PauseLayerModified, PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();

		// if (!Mod::get()->getSettingValue<bool>("enable-mod")) return;

		if (auto menu = this->getChildByID("right-button-menu")) {
			auto setting_button = CCMenuItemSpriteExtra::create(
				CircleButtonSprite::createWithSprite("PauseMenuButton.png"_spr, 1.1f, CircleBaseColor::Green, CircleBaseSize::Tiny),
				this, menu_selector(PauseLayerModified::openMenu)
			);
			menu->addChild(setting_button);
			menu->updateLayout();
		}
	}

	void openMenu(CCObject*) {
		// log::debug("level id: {}", PlayLayer::get()->m_levelSettings->m_level->m_levelID);
		CCSize win_size = CCDirector::sharedDirector()->getWinSize();

		progtracklabel::PLTLayer* menu_layer = progtracklabel::PLTLayer::create(PlayLayer::get()->m_level);
		menu_layer->setID("progresslabeltrack-menulayer");
		menu_layer->setZOrder(105);
		// menu_layer->setAnchorPoint(CCPointMake(0.5f, 0.5f));
		menu_layer->setPosition(CCPointZero);
		menu_layer->setContentSize(win_size);

		auto menu_container = menu_layer->getChildByIndex(0)->getChildByType<CCMenu>(0);

		auto settings_button = CCMenuItemExt::createSpriteExtraWithFrameName(
			"GJ_optionsBtn_001.png", 0.75f, [this](CCMenuItemSpriteExtra*) {
				geode::Popup* mod_settings_popup = openSettingsPopup(Mod::get(), false);
				if (mod_settings_popup != nullptr) {
					Mod::get()->setSavedValue<bool>("seethrough-activated", false);
					CCNode* layer_main;
					if ((layer_main = mod_settings_popup->getChildByIndex(0)) != nullptr)
					if ((layer_main = layer_main->getChildByIndex(1)) != nullptr)
					{
						// for (int i = layer_main->getChildrenCount() - 1; i >= 0 ; i--)
						// 	log::debug("{}", layer_main->getChildByIndex(i));
						auto button_hide_pauselayer = CCMenuItemExt::createSpriteExtra(
							CircleButtonSprite::createWithSpriteFrameName("hideBtn_001.png", 1.f, CircleBaseColor::DarkAqua),
							[this, layer_main](CCMenuItemSpriteExtra*) { this->seeThroughSettingsUpdate(layer_main); }
						);
						button_hide_pauselayer->setEnabled(true);
						button_hide_pauselayer->setPosition(CCPointMake(layer_main->getContentWidth() - 3, layer_main->getContentHeight() - 3));
						// button_hide_pauselayer->setScale(0.5);
						button_hide_pauselayer->setZOrder(101);
						layer_main->addChild(button_hide_pauselayer);
						
						mod_settings_popup->addOnExitCallback([this]() { this->seeThroughSettingsUpdate(nullptr); }, 0);
					}
				}
			}
		);
		settings_button->setPosition(menu_container->getContentSize()+CCSizeMake(-3,-3));
		menu_container->addChild(settings_button);
		
		// CCSprite* background = CCSprite::create("GJ_gradientBG-hd.png");
		// background->setColor(cc3bFromHexString("#6456FF").unwrap());
		
		// background->setScaleX(scene_width/background->getContentWidth());
		// background->setScaleY(scene_height/background->getContentHeight());
				
		// auto setting_button = CCMenuItemSpriteExtra::create(
		// 	// ButtonSprite::create(CCSprite::create("GJ_arrow_01_001.png")),
		// 	CircleButtonSprite::createWithSprite("PauseMenuButton.png"_spr, 1.1f, CircleBaseColor::Green, CircleBaseSize::Tiny),
		// 	this, menu_selector(PauseLayerModified::closeMenu)
		// );
		
		
		// menu_layer->addChild(background);
		// menu_layer->addChild(label1);
		// menu_layer->addChild(setting_button);
		CCLabelBMFont* label_title = CCLabelBMFont::create("Progress Track Label", "goldFont.fnt");
			label_title->setPosition(CCPointMake(184, 260));
			label_title->setAnchorPoint(CCPointMake(.5f, .5f));
			label_title->setScale(0.8);
			menu_container->addChild(label_title);

		// CCLabelBMFont* label_message = CCLabelBMFont::create("Message:", "bigFont.fnt");
		// 	label_message->setPosition(CCPointMake(74, 220));
		// 	label_message->setAnchorPoint(CCPointMake(.5f, .5f));
		// 	label_message->setScale(0.5);
		// 	menu_container->addChild(label_message);
		// TextInput* input_message = TextInput::create(200, "Enter message", "chatFont.fnt");
		// 	input_message->getInputNode()->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890!@#$%^&*()`~-_=+[]{}\\|;:'\",<.>/?");
		// 	input_message->setPosition(CCPointMake(224, 220));
		// 	input_message->setAnchorPoint(CCPointMake(.5f, .5f));
		// 	input_message->setString(Mod::get()->getSettingValue<std::string>("label-main"));
		// 	menu_container->addChild(input_message);

		// CCLabelBMFont* label_newlinereplacement = CCLabelBMFont::create("Newline\nReplacement:", "bigFont.fnt");
		// 	label_newlinereplacement->setPosition(CCPointMake(109, 180));
		// 	label_newlinereplacement->setAnchorPoint(CCPointMake(1.0f, .5f));
		// 	label_newlinereplacement->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
		// 	label_newlinereplacement->setScale(0.3);
		// 	menu_container->addChild(label_newlinereplacement);
		// TextInput* input_newlinereplacement = TextInput::create(50, "newline\nreplacement", "chatFont.fnt");
		// 	input_newlinereplacement->getInputNode()->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()`~-_=+[]{}\\|;:'\",<.>/?");
		// 	input_newlinereplacement->setPosition(CCPointMake(149, 180));
		// 	input_newlinereplacement->setAnchorPoint(CCPointMake(.5f, .5f));
		// 	input_newlinereplacement->setString(Mod::get()->getSettingValue<std::string>("newline-replacement"));
		// 	menu_container->addChild(input_newlinereplacement);

		// CCLabelBMFont* label_counterreplacement = CCLabelBMFont::create("Counter\nReplacement:", "bigFont.fnt");
		// 	label_counterreplacement->setPosition(CCPointMake(259, 180));
		// 	label_counterreplacement->setAnchorPoint(CCPointMake(1.0f, .5f));
		// 	label_counterreplacement->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
		// 	label_counterreplacement->setScale(0.3);
		// 	menu_container->addChild(label_counterreplacement);
		// CCLabelBMFont* label_counterreplacement_2 = CCLabelBMFont::create("(if you write ctr{} -> replaces ctr1, ctr2, etc.)", "chatFont.fnt");
		// 	label_counterreplacement_2->setPosition(CCPointMake(324, 160));
		// 	label_counterreplacement_2->setAnchorPoint(CCPointMake(1.0f, .5f));
		// 	label_counterreplacement_2->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
		// 	label_counterreplacement_2->setScale(0.5);
		// 	menu_container->addChild(label_counterreplacement_2);
		// TextInput* input_counterreplacement = TextInput::create(50, "counter\nreplacement", "chatFont.fnt");
		// 	input_counterreplacement->getInputNode()->setAllowedChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()`~-_=+[]{}\\|;:'\",<.>/?");
		// 	input_counterreplacement->setPosition(CCPointMake(299, 180));
		// 	input_counterreplacement->setAnchorPoint(CCPointMake(.5f, .5f));
		// 	input_counterreplacement->setString(Mod::get()->getSettingValue<std::string>("counter-replacement"));
		// 	menu_container->addChild(input_counterreplacement);

		// CCLabelBMFont* label_position_x = CCLabelBMFont::create("Pos X:", "bigFont.fnt");
		// 	label_position_x->setPosition(CCPointMake(49, 149));
		// 	label_position_x->setAnchorPoint(CCPointMake(1.0f, .5f));
		// 	label_position_x->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
		// 	label_position_x->setScale(0.3);
		// 	menu_container->addChild(label_position_x);
		// TextInput* input_position_x = TextInput::create(50, "X", "bigFont.fnt");
		// 	input_position_x->getInputNode()->setAllowedChars("0123456789");
		// 	input_position_x->setScale(0.5);
		// 	input_position_x->setPosition(CCPointMake(69, 149));
		// 	input_position_x->setAnchorPoint(CCPointMake(.5f, .5f));
		// 	input_position_x->setString(fmt::format("{}", Mod::get()->getSettingValue<int>("position-x")));
		// 	menu_container->addChild(input_position_x);
		// // Slider* slider_position_x = Slider::create(this, menu_selector(PauseLayerModified::sliderPosXMovement));
		// // slider_position_x->setScale(0.4);
		// // slider_position_x->setAnchorPoint(CCPointZero);
		// // slider_position_x->setPosition(CCPointMake(69, 135));
		// // menu_container->addChild(slider_position_x);

		// CCLabelBMFont* label_position_y = CCLabelBMFont::create("Pos Y:", "bigFont.fnt");
		// 	label_position_y->setPosition(CCPointMake(49, 131));
		// 	label_position_y->setAnchorPoint(CCPointMake(1.0f, .5f));
		// 	label_position_y->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
		// 	label_position_y->setScale(0.3);
		// 	menu_container->addChild(label_position_y);
		// TextInput* input_position_y = TextInput::create(50, "Y", "bigFont.fnt");
		// 	input_position_x->getInputNode()->setAllowedChars("0123456789");
		// 	input_position_y->setScale(0.5);
		// 	input_position_y->setPosition(CCPointMake(69, 131));
		// 	input_position_y->setAnchorPoint(CCPointMake(.5f, .5f));
		// 	input_position_y->setString(fmt::format("{}", Mod::get()->getSettingValue<int>("position-y")));
		// 	menu_container->addChild(input_position_y);

		//////////////////////////////////////////////////////////////////////////

		CCLabelBMFont* label_left_counters = CCLabelBMFont::create("Counters", "goldFont.fnt");
			label_left_counters->setPosition(CCPointMake(80, 230));
			label_left_counters->setAnchorPoint(CCPointMake(1.f, .5f));
			label_left_counters->setScale(0.4);
			menu_container->addChild(label_left_counters);
		CCLabelBMFont* label_left_values = CCLabelBMFont::create("Values", "goldFont.fnt");
			label_left_values->setPosition(CCPointMake(80, 200));
			label_left_values->setAnchorPoint(CCPointMake(1.f, .5f));
			label_left_values->setScale(0.4);
			menu_container->addChild(label_left_values);
		CCLabelBMFont* label_left_checkbox_enablebind = CCLabelBMFont::create("Enable\nKeybinds", "goldFont.fnt");
			label_left_checkbox_enablebind->setPosition(CCPointMake(80, 170));
			label_left_checkbox_enablebind->setAnchorPoint(CCPointMake(1.f, .5f));
			label_left_checkbox_enablebind->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
			label_left_checkbox_enablebind->setScale(0.4);
			menu_container->addChild(label_left_checkbox_enablebind);
		CCLabelBMFont* label_left_checkbox_sameattprevent = CCLabelBMFont::create("Prevent bind\nOn same att", "goldFont.fnt");
			label_left_checkbox_sameattprevent->setPosition(CCPointMake(80, 140));
			label_left_checkbox_sameattprevent->setAnchorPoint(CCPointMake(1.f, .5f));
			label_left_checkbox_sameattprevent->setAlignment(CCTextAlignment::kCCTextAlignmentRight);
			label_left_checkbox_sameattprevent->setScale(0.4);
			menu_container->addChild(label_left_checkbox_sameattprevent);
		
		CCLabelBMFont* label_counters[9];
		TextInput* label_inputs[9];
		CCMenuItemToggler* label_checkbox_enablebind[9];
		CCMenuItemToggler* label_checkbox_sameattprevent[9];
		for (int i = 0; i < 9; i++) {
			label_counters[i] = CCLabelBMFont::create(fmt::format("{}",i+1).c_str(), "bigFont.fnt");
				label_counters[i]->setPosition(CCPointMake(104+30*i, 230));
				label_counters[i]->setAnchorPoint(CCPointMake(.5f, .5f));
				label_counters[i]->setScale(0.5);
				menu_container->addChild(label_counters[i]);
			label_inputs[i] = TextInput::create(40, "", "bigFont.fnt");
				label_inputs[i]->getInputNode()->setAllowedChars("1234567890");
				label_inputs[i]->setScale(0.5);
				label_inputs[i]->setPosition(CCPointMake(104+30*i, 200));
				label_inputs[i]->setAnchorPoint(CCPointMake(.5f, .5f));
				label_inputs[i]->setString(fmt::format("{}",Mod::get()->getSavedValue<int>(fmt::format("counter-{}", i+1))));
				label_inputs[i]->setID(fmt::format("label-input-{}", i+1));
				menu_container->addChild(label_inputs[i]);
			label_checkbox_enablebind[i] = CCMenuItemToggler::createWithSize("GJ_checkOff_001.png", "GJ_checkOn_001.png", this, nullptr, 0.5);
				label_checkbox_enablebind[i]->setPosition(CCPointMake(104+30*i, 170));
				label_checkbox_enablebind[i]->setID(fmt::format("label-checkbox-enablebind-{}", i+1));
				label_checkbox_enablebind[i]->toggle(Mod::get()->getSavedValue<bool>(fmt::format("counter-enablebind-{}", i+1)));
				menu_container->addChild(label_checkbox_enablebind[i]);
				label_checkbox_sameattprevent[i] = CCMenuItemToggler::createWithSize("GJ_checkOff_001.png", "GJ_checkOn_001.png", this, nullptr, 0.5);
				label_checkbox_sameattprevent[i]->setPosition(CCPointMake(104+30*i, 140));
				label_checkbox_sameattprevent[i]->setID(fmt::format("label-checkbox-sameattprevent-{}", i+1));
				label_checkbox_sameattprevent[i]->toggle(Mod::get()->getSavedValue<bool>(fmt::format("counter-sameattprevent-{}", i+1)));
				menu_container->addChild(label_checkbox_sameattprevent[i]);
		}

		auto button_save_counters = CCMenuItemExt::createSpriteExtra(
			ButtonSprite::create("Save","bigFont.fnt","GJ_button_04.png", 0.5), [this](CCMenuItemSpriteExtra*) {
				for (int i = 1; i < 10; i++) {
					TextInput* text_input = (TextInput*) CCScene::get()->getChildByIDRecursive(fmt::format("label-input-{}", i));
					
					auto result = numFromString<int>(text_input->getString());
					if (result.isOk()) {
						Mod::get()->setSavedValue<int>(fmt::format("counter-{}", i), result.ok().value());
						auto layer_ = (progtracklabel::LabelLayer*) CCScene::get()->getChildByIDRecursive("progresstracklabel-labellayer");
						if (layer_ != nullptr) {
							auto label_main = (CCLabelBMFont*) layer_->getChildByID("plt-text-label");
							if (label_main != nullptr)
							progtracklabel::reset_layer::set_label_text(label_main);
						}
					}
					CCMenuItemToggler* label_checkbox_enablebind = (CCMenuItemToggler*) CCScene::get()->getChildByIDRecursive(fmt::format("label-checkbox-enablebind-{}", i));
					Mod::get()->setSavedValue<bool>(fmt::format("counter-enablebind-{}", i), label_checkbox_enablebind->isToggled());
					CCMenuItemToggler* label_checkbox_sameattprevent = (CCMenuItemToggler*) CCScene::get()->getChildByIDRecursive(fmt::format("label-checkbox-sameattprevent-{}", i));
					Mod::get()->setSavedValue<bool>(fmt::format("counter-sameattprevent-{}", i), label_checkbox_sameattprevent->isToggled());
				}
			}
		);
		button_save_counters->setPosition(320, 35);
		menu_container->addChild(button_save_counters);
		
		CCScene::get()->addChild(menu_layer);
	}

	void seeThroughSettingsUpdate(CCNode* layer_main) {
		bool seethrough_activated = (layer_main == nullptr) || Mod::get()->getSavedValue<bool>("seethrough-activated");
		PauseLayer* pauselayer;
		if ((pauselayer = CCScene::get()->getChildByType<PauseLayer>(0)) != nullptr)
			pauselayer->setVisible(seethrough_activated);
		auto ptllayer = (progtracklabel::PLTLayer*) CCScene::get()->getChildByID("progresslabeltrack-menulayer");
		if (ptllayer != nullptr)
			ptllayer->setVisible(seethrough_activated);
		if (layer_main == nullptr) return;
		CCLayer* layer_parent = (CCLayer*) layer_main->getParent();
		auto sprite = layer_parent->getChildByType<NineSlice>(0);
		// auto sprite = layer_parent->getChildByType<extension::CCScale9Sprite>(0);  // Geode v4.x Legacy code
		if (sprite != nullptr)
			sprite->setVisible(seethrough_activated);
		Mod::get()->setSavedValue<bool>("seethrough-activated", !seethrough_activated);
	}

	void sliderPosXMovement(CCObject* object) {
		// log::debug("{}", object->m_nZOrder);
	}

	void closeMenu(CCObject*) {
		CCScene::get()->removeChildByID("progresslabeltrack-menulayer");
	}
};

class $modify(PlayLayerModified, PlayLayer) {
	void resetLevel() {
		log::debug("ProgressTrackLabel tracked level reset; you can now use the number keybinds again");
		for (int i = 1; i < 10; i++)
			Mod::get()->setSavedValue<bool>(fmt::format("counter-available-{}", i), true);
		PlayLayer::resetLevel();
	}
};