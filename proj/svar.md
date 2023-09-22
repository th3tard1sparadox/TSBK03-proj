# Hur ser högdagrarna ut i 1a? Varför?
    Vita
# Hur allokerar man en tom textur?
    `glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);`
# Hur många pass körde du lågpassfiltret i 1c och 1d?
    Typ 200
# Hur löste du 1f (om du gjorde det)? Vilket/vilka filter? (Extrauppgift)
    Nej
# Bör trunkeringen göras i egen shader eller som del av en shader som gör något mer? Varför?
    Vi vill göra det i separata shaders då vi också vill behålla originalbilden