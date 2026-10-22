#pragma once

#define TEXT_HEIGHT (16)
#define TEXT_PAGES (2)

#define TEXT_WELCOME_WIDTH (56)
#define TEXT_LED_WIDTH (24)
#define TEXT_IMAGE_WIDTH (40)
#define TEXT_ANIME_WIDTH (40)
#define TEXT_GAME_WIDTH (32)
#define TEXT_GAME_OVER_WIDTH (72)
#define TEXT_SCORE_WIDTH (40)

#define NUMBER_WIDTH (8)

#define SYMBOL_SLASH_WIDTH (8)

#define TEXT_WELCOME_SIZE (TEXT_WELCOME_WIDTH * TEXT_PAGES)
#define TEXT_LED_SIZE (TEXT_LED_WIDTH * TEXT_PAGES)
#define TEXT_IMAGE_SIZE (TEXT_IMAGE_WIDTH * TEXT_PAGES)
#define TEXT_ANIME_SIZE (TEXT_ANIME_WIDTH * TEXT_PAGES)
#define TEXT_GAME_SIZE (TEXT_GAME_WIDTH * TEXT_PAGES)
#define TEXT_GAME_OVER_SIZE (TEXT_GAME_OVER_WIDTH * TEXT_PAGES)
#define TEXT_SCORE_SIZE (TEXT_SCORE_WIDTH * TEXT_PAGES)

#define NUMBER_0_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_1_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_2_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_3_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_4_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_5_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_6_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_7_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_8_SIZE (NUMBER_WIDTH * TEXT_PAGES)
#define NUMBER_9_SIZE (NUMBER_WIDTH * TEXT_PAGES)

#define SYMBOL_SLASH_SIZE (SYMBOL_SLASH_WIDTH * TEXT_PAGES)

extern const unsigned char text_welcome[TEXT_WELCOME_SIZE];
extern const unsigned char text_led[TEXT_LED_SIZE];
extern const unsigned char text_image[TEXT_IMAGE_SIZE];
extern const unsigned char text_anime[TEXT_ANIME_SIZE];
extern const unsigned char text_game[TEXT_GAME_SIZE];
extern const unsigned char text_game_over[TEXT_GAME_OVER_SIZE];
extern const unsigned char text_score[TEXT_SCORE_SIZE];

extern const unsigned char number_0[NUMBER_0_SIZE];
extern const unsigned char number_1[NUMBER_1_SIZE];
extern const unsigned char number_2[NUMBER_2_SIZE];
extern const unsigned char number_3[NUMBER_3_SIZE];
extern const unsigned char number_4[NUMBER_4_SIZE];
extern const unsigned char number_5[NUMBER_5_SIZE];
extern const unsigned char number_6[NUMBER_6_SIZE];
extern const unsigned char number_7[NUMBER_7_SIZE];
extern const unsigned char number_8[NUMBER_8_SIZE];
extern const unsigned char number_9[NUMBER_9_SIZE];

extern const unsigned char symbol_slash[SYMBOL_SLASH_SIZE];
