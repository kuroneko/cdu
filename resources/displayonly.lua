local font = require('aerowinx-font')

return {
    dimensions = {
        offset = {-- no offset automatically centers the CDU in the available space
            -- x = -1;
            -- y = -1;
        },
        size = {
            -- no size automatically scales the CDU to fit available space
            w = -1;
            h = -1;
        },
    },
    display = {
        position = {
            w = 522,
            h = 426,
        };
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
            keyboard = { "INS" };
        },
        MENU = {
            keyboard = { "HOME" };
        }
    }
}