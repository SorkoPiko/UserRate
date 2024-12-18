#include <Geode/modify/ProfilePage.hpp>

#include "../layers/LoadLayer.hpp"
#include "../managers/Global.hpp"
#include "../managers/API.hpp"

using namespace geode::prelude;

class $modify(RateProfilePage, ProfilePage) {
    struct Fields {
        CCMenuItemSpriteExtra* reassignButton = nullptr;
    };

    void loadPageFromUserInfo(GJUserScore* user) {
        ProfilePage::loadPageFromUserInfo(user);

        const auto global = Global::getInstance();
        if (!global->isAdmin()) return;

        if (const auto leftMenu = m_mainLayer->getChildByID("left-menu")) {
            if (!global->isUserChecked(m_accountID))
                API::isModerator(m_accountID, [this, leftMenu, global](const bool mod, const bool admin, const std::string& message) {
                    if (!message.empty()) {
                        FLAlertLayer::create(
                            "Error",
                            fmt::format("Failed to check moderator status: {}.", message),
                            "OK"
                        )->show();
                    }

                    if (mod) global->addMod(m_accountID);
                    if (admin) global->addAdmin(m_accountID);
                    global->addChecked(m_accountID);
                    log::info("{} {} {}", m_accountID, mod, admin);

                    if (!admin) {
                        const auto sprite = CircleButtonSprite::create(
                            CCSprite::createWithSpriteFrameName("modBadge_01_001.png"),
                            CircleBaseColor::Cyan
                        );
                        sprite->setScale(0.7f);

                        m_fields->reassignButton = CCMenuItemSpriteExtra::create(
                            sprite,
                            this,
                            menu_selector(RateProfilePage::onReassign)
                        );
                        m_fields->reassignButton->setID("reassign-button"_spr);

                        leftMenu->addChild(m_fields->reassignButton);
                        m_buttons->addObject(m_fields->reassignButton);

                        leftMenu->updateLayout();
                    }
                });
            else if (!global->isUserAdmin(m_accountID)) {
                const auto sprite = CircleButtonSprite::create(
                    CCSprite::createWithSpriteFrameName("modBadge_01_001.png"),
                    CircleBaseColor::Cyan
                );
                sprite->setScale(0.7f);

                m_fields->reassignButton = CCMenuItemSpriteExtra::create(
                    sprite,
                    this,
                    menu_selector(RateProfilePage::onReassign)
                );
                m_fields->reassignButton->setID("reassign-button"_spr);

                leftMenu->addChild(m_fields->reassignButton);
                m_buttons->addObject(m_fields->reassignButton);

                leftMenu->updateLayout();
            }
        }
    }

    void onReassign(CCObject*) {
        const auto global = Global::getInstance();
        const bool isMod = global->isUserMod(m_accountID);

        std::string popupContent;
        std::string successMessage;
        std::string failMessage;
        if (isMod) {
            popupContent = "Revoke this user's moderator status?";
            successMessage = "Success! The user's moderator status has been revoked.";
            failMessage = "Failed to revoke user's moderator status";
        } else {
            popupContent = "Grant this user moderator status?";
            successMessage = "Success! The user has been granted moderator status.";
            failMessage = "Failed to grant user moderator status";
        }

        const auto popup = createQuickPopup(
            "Reassign user",
            popupContent,
            "Cancel", "Confirm",
            [this, successMessage, failMessage, isMod, global](auto, const bool btn2) {

                if (btn2) {
                    const auto loadLayer = LoadLayer::create();
                    loadLayer->show();
                    API::reassignModerator(m_accountID, !isMod, [this, loadLayer, successMessage, failMessage, global](const bool success, const std::string& message) {
                        loadLayer->finished();

                        if (success) {
                            FLAlertLayer::create(
                                "Reassign user",
                                successMessage,
                                "OK"
                            )->show();

                            global->clearUser(m_accountID);
                            if (m_fields->reassignButton) {
                                m_fields->reassignButton->removeFromParent();
                            }
                        }

                        else
                            FLAlertLayer::create(
                                "Reassign user",
                                fmt::format("{}: {}.", failMessage, message),
                                "OK"
                            )->show();
                    });
                }
            }
        );

        popup->m_button1->updateBGImage("GJ_button_06.png");
    }
};