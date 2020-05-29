--
-- Created by IntelliJ IDEA.
-- User: chris
-- Date: 29/05/2020
-- Time: 2:34 pm
-- To change this template use File | Settings | File Templates.
--

local font = require('aerowinx-font')
return {
    dimensions = {
        offset = {-- no offset automatically centers the CDU in the available space

            -- x = -1,
            -- y = -1,
        },
        size = {--  no size automatically scales the CDU to fit available space

            -- w = - 1,
            -- h = - 1,
        },
    },
    background = {
        image = "resources/MCDU_1170_blank.png",
    },
    display = {
        -- for MCDU_1170, the display offsets are:
        -- 114, 79
        -- sizes are
        -- 522, 426
        position = {
            x = 114,
            y = 79,
            w = 522,
            h = 426,
        },
        font = font,
        -- we need to blank out the background always.
        fill_background = false,
        default_colour = {
            -- accepts "green", "black", "white", "amber", "red", "cyan", "magenta"
            foreground = "green",
            -- background also accepts "none" in which instance no cell background will be rendered
            -- background = "none",
        },
    },
    input = {
        INIT_REF = {
            keyboard = { "INS" },
            region = {
                x = 89,
                y = 549,
                w = 89,
                h = 67,
            },
        },
        RTE = {
            region = {
                x = 185,
                y = 549,
                w = 89,
                h = 67,
            },
        },
        DEP_ARR = {
            region = {
                x = 280,
                y = 548,
                w = 89,
                h = 67,
            },
        },
        ATC = {
            region = {
                x = 376,
                y = 548,
                w = 89,
                h = 67,
            },
        },
        VNAV = {
            region = {
                x = 472,
                y = 548,
                w = 89,
                h = 67,
            },
        },
        PROG = {
            region = {
                x = 472,
                y = 621,
                w = 89,
                h = 67,
            },
        },
        MENU = {
            keyboard = { "HOME" },
            region = {
                x = 581,
                y = 620,
                w = 89,
                h = 67,
            },
        },
    },
}