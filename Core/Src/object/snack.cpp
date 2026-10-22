#include "object/snack.hpp"
#include "driver/OLED.hpp"

void Snack::start()
{
    move_after_frames = 30;
    body_.push_back({3, 6});
    body_.push_back({3, 7});
}

void Snack::update()
{
    if (!active)
        return;
    if (frame_num_ < move_after_frames)
    {
        frame_num_++;
        return;
    }

    frame_num_ = 0;
    // 往后方转，撤销本次操作
    if (static_cast<int8_t>(dire_cache_) == static_cast<int8_t>(dire_))
        dire_ = dire_cache_;
    else
        dire_cache_ = dire_;

    // 移动
    body_.splice(body_.begin(), body_, --body_.end());

    // 绘制
    for (auto i : body_)
    {
        OLED::DrawRectangle(i.x * 8, i.y * 8, 8, 8, 1, 1);
    }
}