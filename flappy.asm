.ORIG x3000

LOOP

; delete previous pixel 
    LD R1, PIXEL_POS
    LD R2, BLACK
    STR R2, R1, #0

; delete previous wall
    LD R1, VRAM_BASE
    LD R2, WALL_COL
    ADD R1, R1, R2
    LD R3, BLACK
    LD R4, ROW_LENGTH
    LD R5, ROW_LENGTH

DELETE_WALL
    STR R3, R1, #0
    ADD R1, R1, R5
    ADD R4, R4, #-1
    BRp DELETE_WALL

; move wall to left
    LD R2, WALL_COL
    ADD R2, R2, #-1
    BRzp SAVE

    LDI R6, SCORE_ADDR
    ADD R6, R6, #1
    STI R6, SCORE_ADDR

    LD R2, WALL_RESET

; generate new gap
    LDI R0, RND_ADDR
    LD R1, RND_ROW
    AND R0, R0, R1
    LD R1, VRAM_BASE
    ADD R0, R0, R1
    LD R1, MIN_DIST
    ADD R0, R0, R1
    ST R0, GAP_START_POS

SAVE
    ST R2, WALL_COL

; new wall
    LD R1, VRAM_BASE
    LD R2, WALL_COL
    ADD R1, R1, R2
    LD R3, WALL_COLOR
    LD R4, ROW_LENGTH
    LD R5, ROW_LENGTH

CREATE_WALL
    STR R3, R1, #0
    ADD R1, R1, R5
    ADD R4, R4, #-1
    BRp CREATE_WALL

; new gap
    LD R1, GAP_START_POS
    LD R2, WALL_COL
    ADD R1, R1, R2
    LD R3, BLACK
    LD R4, GAP_HEIGHT
    LD R5, ROW_LENGTH

CREATE_GAP
    STR R3, R1, #0
    ADD R1, R1, R5
    ADD R4, R4, #-1
    BRp CREATE_GAP

; pixel flying process
    LD R1, PIXEL_POS
    LDI R0, KBSR
    BRzp GRAVITY

    LDI R0, KBDR
    LD R2, CHAR_W
    ADD R2, R0, R2
    BRnp GRAVITY

    LD R2, JUMP
    ADD R1, R1, R2
    BRnzp MOVE

GRAVITY
    LD R2, ROW_LENGTH
    ADD R1, R1, R2

MOVE
    ST R1, PIXEL_POS

; pixel collision
; 1. if the pixel hits the top of the screen
    LD R2, VRAM_TOP
    ADD R2, R1, R2
    BRn GAME_OVER

; 2. if the pixel hits the bottom of the screen
    LD R2, VRAM_BOTTOM
    ADD R2, R1, R2
    BRzp GAME_OVER

; 3. if the pixel hits the wall
    LDR R2, R1, #0
    LD R3, WALL_COLOR
    NOT R3, R3
    ADD R3, R3, #1
    ADD R2, R2, R3
    BRz GAME_OVER

; set pixel color, delay and score 
    LD R2, PIXEL_COLOR
    STR R2, R1, #0

    LD R6, DELAY_VAL
DELAY
    ADD R6, R6, #-1
    BRp DELAY
    BRnzp LOOP

GAME_OVER
    BRnzp GAME_OVER

PIXEL_POS .FILL #-13814 ; pixel start position (xCA0A)
WALL_COL .FILL #63 ; wall start position
GAP_START_POS .FILL #-15360 ; gap start position in the wall
GAP_HEIGHT .FILL #10 ; gap height

VRAM_BASE .FILL #-16384 ; screen base address
VRAM_TOP .FILL #16384 ; screen top address
VRAM_BOTTOM .FILL #12288 ; screen bottom address
RND_ADDR .FILL #-508 ; random address
RND_ROW .FILL #1984 ; random row address
MIN_DIST .FILL #256 ; minimum distance from the top of the screen

BLACK .FILL #0 ; color for deleting the pixel
WALL_COLOR .FILL #992 ; color for wall
PIXEL_COLOR .FILL #1023 ; color for pixel

ROW_LENGTH .FILL #64 ; row length
WALL_RESET .FILL #63 ; resetting the wall to the right
JUMP .FILL #-192 ; pixel jump (3 rows)
CHAR_W .FILL #-119 ; W character

KBSR .FILL #-512 ; keyboard status
KBDR .FILL #-510 ; keyboard dreapta
DELAY_VAL .FILL #8500 ; game speed
SCORE_ADDR .FILL #16384 ; game score 

.END
