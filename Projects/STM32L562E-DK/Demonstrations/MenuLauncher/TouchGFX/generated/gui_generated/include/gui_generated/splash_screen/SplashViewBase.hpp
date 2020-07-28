/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef SPLASHVIEWBASE_HPP
#define SPLASHVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/splash_screen/SplashPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/mixins/FadeAnimator.hpp>

class SplashViewBase : public touchgfx::View<SplashPresenter>
{
public:
    SplashViewBase();
    virtual ~SplashViewBase() {}
    virtual void setupScreen();
    virtual void handleTickEvent();
    virtual void afterTransition();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box background;
    touchgfx::FadeAnimator< touchgfx::Image > st_logo;
    touchgfx::Image lifeaugmented;

private:

    /*
     * Delay Variable Declarations
     */
    static const uint16_t WAITINTERACTION_DURATION = 60;
    uint16_t waitInteractionCounter;
    /*
     * Interaction Callback Declarations
     */
    touchgfx::Callback < SplashViewBase, const touchgfx::FadeAnimator<touchgfx::Image>& >  fadeInLogoEndedCallback;


    /*
     * Interaction Handlers
     */
    void fadeInLogoEndedCallbackHandler(const touchgfx::FadeAnimator<touchgfx::Image>& comp);

};

#endif // SPLASHVIEWBASE_HPP