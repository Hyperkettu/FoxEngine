#include "Fox.h"
#include "WindowCaption.h"


namespace Fox {

	namespace Platform {

		namespace Win32 {

			VOID WindowCaption::AddCaptionButton(CaptionButton* button) {
				captionButtons.push_back(button);
			}


		}
	}
}