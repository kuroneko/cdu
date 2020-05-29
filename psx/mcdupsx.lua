-- mcdupsx configuration file
local photoreal = require('photoreal')

return {
    psx = {
        hostname = "localhost",
        port = 10747,
    },
    mcdu = photoreal,
    window = {
        x = 1024,
        y = 768,
        fullscreen = false,
    },
    position = "left",
}
