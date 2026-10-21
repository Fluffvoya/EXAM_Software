#pragma once

#include "main.h"
#include "object.hpp"

struct LED : public Object
{
    LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
    void start() override;
    void update() override;

    bool light{false};

private:
    GPIO_TypeDef *GPIOx_;
    uint16_t GPIO_Pin_;
};
