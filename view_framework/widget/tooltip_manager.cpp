
#include "tooltip_manager.h"

#include <vector>

#include "gfx/font.h"

namespace view
{

    // Maximum number of characters we allow in a tooltip.
    static const size_t kMaxTooltipLength = 1024;

    // Maximum number of lines we allow in the tooltip.
    static const size_t kMaxLines = 6;

    // Breaks |text| along line boundaries, placing each line of text into lines.
    static void SplitTooltipString(const std::wstring& text,
        std::vector<std::wstring>* lines)
    {
        size_t index = 0;
        size_t next_index;
        while((next_index=text.find(TooltipManager::GetLineSeparator(), index))
            !=std::wstring::npos && lines->size()<kMaxLines)
        {
            lines->push_back(text.substr(index, next_index-index));
            index = next_index + TooltipManager::GetLineSeparator().size();
        }
        if(next_index!=text.size() && lines->size()<kMaxLines)
        {
            lines->push_back(text.substr(index, text.size()-index));
        }
    }

    // static
    void TooltipManager::TrimTooltipToFit(std::wstring* text,
        int* max_width,
        int* line_count,
        int x,
        int y)
    {
        *max_width = 0;
        *line_count = 0;

        // Clamp the tooltip length to kMaxTooltipLength so that we don't
        // accidentally DOS the user with a mega tooltip.
        if(text->length() > kMaxTooltipLength)
        {
            *text = text->substr(0, kMaxTooltipLength);
        }

        // Determine the available width for the tooltip.
        int available_width = GetMaxWidth(x, y);

        // Split the string.
        std::vector<std::wstring> lines;
        SplitTooltipString(*text, &lines);
        *line_count = static_cast<int>(lines.size());

        // Format each line to fit.
        gfx::Font font = GetDefaultFont();
        std::wstring result;
        for(std::vector<std::wstring>::iterator i=lines.begin();
            i!=lines.end(); ++i)
        {
            // WLW TODO: �滻tooltip.
            std::wstring elided_text = *i;//gfx::ElideText(*i, font, available_width, false);
            *max_width = std::max(*max_width, font.GetStringWidth(elided_text));
            if(i==lines.begin() && i+1==lines.end())
            {
                *text = elided_text;
                return;
            }
            if(!result.empty())
            {
                result.append(GetLineSeparator());
            }
            result.append(elided_text);
        }
        *text = result;
    }

} //namespace view