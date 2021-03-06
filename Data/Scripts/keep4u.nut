auth_token <- "";
function regex_simple(data, regStr, start) {
	local ex = regexp(regStr);
	local res = ex.capture(data, start);
	local resultStr = "";
	if (res != null) {
		resultStr = data.slice(res[1].begin, res[1].end);
	}
	return resultStr;
}

function _WriteLog(type,message) {
	try {
		WriteLog(type, message);
	} catch (ex ) {
		print(type + " : " + message);
	}
}
function ObtainToken() {
    if (auth_token == "") {
        nm.doGet("http://keep4u.ru/");
        if (nm.responseCode() != 200) {
            return "";
        }
        auth_token = regex_simple(nm.responseBody(), "auth_token = \"(.+)\"", 0);
        return auth_token;
    }
    return auth_token;
}
function UploadFile(FileName, options) {
    try {
			nm.enableResponseCodeChecking(false);
		} catch ( ex ) {} 

	local name = ExtractFileName(FileName);
	local mime = GetFileMimeType(name);
    local token = ObtainToken();
    if (token == "") {
        _WriteLog("error", "[keep4u] Unable to obtain auth token");
		
        return 0;
    }
	nm.setUrl("http://keep4u.ru/json");
	nm.addQueryParam("type", "file");
	nm.addQueryParam("action", "upload");
	nm.addQueryParam("privacy", "public");
	nm.addQueryParam("timestamp", time() + "000");
	nm.addQueryParam("auth_token", token);
	nm.addQueryParam("category_id", "");
	nm.addQueryParam("nswd", "");
	nm.addQueryParamFile("source", FileName, name, mime);
	nm.doUploadMultipartData();
	if (nm.responseCode() == 200) {
		local sJSON = nm.responseBody();
		local t = ParseJSON(sJSON);
		if (t != null) {
			options.setViewUrl(t.image.url_viewer);
			options.setDirectUrl(t.image.url);
			options.setThumbUrl(t.image.thumb.url);
			return 1;
		} else
			return 0;

	} else {
        local t = ParseJSON(nm.responseBody());
        if (t != null) {
			_WriteLog("error", "[keep4u] got error from server: \nResponse code:" + nm.responseCode() + "\n" + t.error.message);
		}
        return 0;
    }
}