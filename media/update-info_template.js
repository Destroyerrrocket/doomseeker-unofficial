// This is a template update information file. It shows how the file should
// be structured.
//
// IMPORTANT: The QtJson parser doesn't support comments so they need to be
//            removed before the file can be used by the program.
//
// 1. The filename should fit the pattern "update-info_platform.js",
//    for example: "update-info_win32.js".
// 2. Plugin names should be lower-cased strings which are returned by plugin's
//    EnginePlugin::name() method. Names should be prepended with "p-" prefix.
//    For example: "p-zandronum"
// 3. "revision" field is for version comparison. "display-version" is for
//    displaying the version for the user in a message box. If "display-version"
//    is omitted, "revision" will be displayed.
// 4. If "stable" version gets released that is newer than "beta" then "beta"
//    must be updated to contain the same information as "stable".
{
"doomseeker" : {
    "stable" : {
        "revision" : 900,
        "display-name" : "Doomseeker",
        "display-version" : "1.0",
        "URL" : "http://127.0.0.1/path/to/update/archive.zip"
    },
    "beta" : {
        "revision" : 902,
        "display-name" : "Doomseeker",
        "URL" : "http://127.0.0.1/path/to/update/archive-beta.zip"
    }
},
"p-zandronum" : {
    "stable" : {
        "revision" : 9,
        "display-name" : "Zandronum",
        "URL" : "http://127.0.0.1/path/to/update/plugin-archive.zip"
    },
    "beta" : {
        // ...
    }
},
"p-chocolatedoom" : {
    // ...
}
// EOF
}
