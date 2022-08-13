#pragma once

namespace Fox {

	namespace Platform {
	
		namespace Win32 { 

			class WindowCaption
			{
				public:

					struct CaptionButton {

						enum Command {
							CLOSE = 0,
							MAXIMIZE = 1,
							MINIMIZE = 2
						};

						std::wstring text;
						INT command = -1;
						INT width;
						RECT rect;

						CaptionButton(std::wstring buttonText, INT buttonCommand, INT buttonWidth = 50) {
							text = buttonText;
							command = buttonCommand;
							width = buttonWidth;
						}
					};

					BOOL ShowTitle() { return showTitle; }
					VOID ShowTitle(BOOL show) { showTitle = show; }

					VOID AddCaptionButton(CaptionButton* button);
					std::list<CaptionButton*> CaptionButtons() { return captionButtons; }
				private:

					BOOL showTitle = TRUE;
					std::list<CaptionButton*> captionButtons;
			};

		}
	}
}

