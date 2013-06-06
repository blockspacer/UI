
#include "accelerator_handler.h"

#include "../app/keyboard_code_conversion_win.h"
#include "../app/keyboard_codes_win.h"
#include "../app/event.h"
#include "focus_manager.h"

namespace view
{

    AcceleratorHandler::AcceleratorHandler() {}

    bool AcceleratorHandler::Dispatch(const MSG& msg)
    {
        bool process_message = true;

        if(msg.message>=WM_KEYFIRST && msg.message<=WM_KEYLAST)
        {
            FocusManager* focus_manager =
                FocusManager::GetFocusManagerForNativeView(msg.hwnd);
            if(focus_manager)
            {
                switch(msg.message)
                {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                    {
                        KeyEvent event(Event::ET_KEY_PRESSED,
                            view::KeyboardCodeForWindowsKeyCode(msg.wParam),
                            KeyEvent::GetKeyStateFlags(),
                            msg.lParam&0xFFFF,
                            (msg.lParam&0xFFFF0000)>>16);
                        process_message = focus_manager->OnKeyEvent(event);
                        if(!process_message)
                        {
                            // Record that this key is pressed so we can remember not to
                            // translate and dispatch the associated WM_KEYUP.
                            pressed_keys_.insert(msg.wParam);
                        }
                        break;
                    }
                case WM_KEYUP:
                case WM_SYSKEYUP:
                    {
                        std::set<WPARAM>::iterator iter = pressed_keys_.find(msg.wParam);
                        if(iter != pressed_keys_.end())
                        {
                            // Don't translate/dispatch the KEYUP since we have eaten the
                            // associated KEYDOWN.
                            pressed_keys_.erase(iter);
                            return true;
                        }
                        break;
                    }
                }
            }
        }

        if(process_message)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return true;
    }

} //namespace view