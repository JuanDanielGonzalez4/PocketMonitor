idf_component_register(SRCS "main.c" "wifi_app.c" "http_server.c" "adc.c"
                    INCLUDE_DIRS "."
                    EMBED_FILES webpage/app.css webpage/app.js webpage/favicon.ico webpage/index.html webpage/jquery-3.3.1.min.js)

                