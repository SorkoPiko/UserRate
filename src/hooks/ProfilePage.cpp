#include <Geode/modify/ProfilePage.hpp>

#include "../managers/Global.hpp"
#include "../managers/API.hpp"

using namespace geode::prelude;

class $modify(RateProfilePage, ProfilePage) {
    struct Fields {
        CCMenuItemSpriteExtra* reassignButton = nullptr;
        EventListener<web::WebTask> listener;
    };

    void onClose(CCObject* sender) {
        ProfilePage::onClose(sender);
    }

    void loadPageFromUserInfo(GJUserScore* user) {
        ProfilePage::loadPageFromUserInfo(user);

        const auto global = Global::get();
        if (!global->isAdmin()) return;

        if (const auto leftMenu = m_mainLayer->getChildByID("left-menu")) {
            if (!global->isUserAdmin(m_accountID)) {
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
        const auto global = Global::get();
        const bool isMod = global->isUserMod(m_accountID);

        std::string popupContent;
        std::string successMessage;
        if (isMod) {
            popupContent = "Revoke this user's moderator status?";
            successMessage = "Success! The user's moderator status has been revoked.";
        } else {
            popupContent = "Grant this user moderator status?";
            successMessage = "Success! The user has been granted moderator status.";
        }

        const auto popup = createQuickPopup(
            "Reassign user",
            popupContent,
            "Cancel", "Confirm",
            [this, successMessage, isMod, global](auto, const bool btn2) {

                if (btn2) {
                    API::reassignModerator(m_accountID, !isMod, [this, successMessage, global](const bool success) {
                        if (success) {
                            FLAlertLayer::create(
                                "Reassign user",
                                successMessage,
                                "OK"
                            )->show();

                            if (m_fields->reassignButton) m_fields->reassignButton->removeFromParentAndCleanup(true);
                        }
                    }, m_fields->listener);
                }
            }
        );

        popup->m_button1->updateBGImage("GJ_button_06.png");
    }
};