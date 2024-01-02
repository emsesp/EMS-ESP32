#include "PsychicStaticFileHandler.h"

/*************************************/
/*  PsychicStaticFileHandler         */
/*************************************/

PsychicStaticFileHandler::PsychicStaticFileHandler(const char* uri, FS& fs, const char* path, const char* cache_control)
  : _fs(fs), _uri(uri), _path(path), _default_file("index.html"), _cache_control(cache_control), _last_modified("")
{
  // Ensure leading '/'
  if (_uri.length() == 0 || _uri[0] != '/') _uri = "/" + _uri;
  if (_path.length() == 0 || _path[0] != '/') _path = "/" + _path;

  // If path ends with '/' we assume a hint that this is a directory to improve performance.
  // However - if it does not end with '/' we, can't assume a file, path can still be a directory.
  _isDir = _path[_path.length()-1] == '/';

  // Remove the trailing '/' so we can handle default file
  // Notice that root will be "" not "/"
  if (_uri[_uri.length()-1] == '/') _uri = _uri.substring(0, _uri.length()-1);
  if (_path[_path.length()-1] == '/') _path = _path.substring(0, _path.length()-1);

  // Reset stats
  _gzipFirst = false;
  _gzipStats = 0xF8;
}

PsychicStaticFileHandler& PsychicStaticFileHandler::setIsDir(bool isDir){
  _isDir = isDir;
  return *this;
}

PsychicStaticFileHandler& PsychicStaticFileHandler::setDefaultFile(const char* filename){
  _default_file = String(filename);
  return *this;
}

PsychicStaticFileHandler& PsychicStaticFileHandler::setCacheControl(const char* cache_control){
  _cache_control = String(cache_control);
  return *this;
}

PsychicStaticFileHandler& PsychicStaticFileHandler::setLastModified(const char* last_modified){
  _last_modified = String(last_modified);
  return *this;
}

PsychicStaticFileHandler& PsychicStaticFileHandler::setLastModified(struct tm* last_modified){
  char result[30];
  strftime (result,30,"%a, %d %b %Y %H:%M:%S %Z", last_modified);
  return setLastModified((const char *)result);
}

bool PsychicStaticFileHandler::canHandle(PsychicRequest *request)
{
  if(request->method() != HTTP_GET || !request->uri().startsWith(_uri) )
    return false;

  if (_getFile(request))
    return true;

  return false;
}

bool PsychicStaticFileHandler::_getFile(PsychicRequest *request)
{
  // Remove the found uri
  String path = request->uri().substring(_uri.length());

  // We can skip the file check and look for default if request is to the root of a directory or that request path ends with '/'
  bool canSkipFileCheck = (_isDir && path.length() == 0) || (path.length() && path[path.length()-1] == '/');

  path = _path + path;

  // Do we have a file or .gz file
  if (!canSkipFileCheck && _fileExists(path))
    return true;

  // Can't handle if not default file
  if (_default_file.length() == 0)
    return false;

  // Try to add default file, ensure there is a trailing '/' ot the path.
  if (path.length() == 0 || path[path.length()-1] != '/')
    path += "/";
  path += _default_file;

  return _fileExists(path);
}

#define FILE_IS_REAL(f) (f == true && !f.isDirectory())

bool PsychicStaticFileHandler::_fileExists(const String& path)
{
  bool fileFound = false;
  bool gzipFound = false;

  String gzip = path + ".gz";

  if (_gzipFirst) {
    _file = _fs.open(gzip, "r");
    gzipFound = FILE_IS_REAL(_file);
    if (!gzipFound){
      _file = _fs.open(path, "r");
      fileFound = FILE_IS_REAL(_file);
    }
  } else {
    _file = _fs.open(path, "r");
    fileFound = FILE_IS_REAL(_file);
    if (!fileFound){
      _file = _fs.open(gzip, "r");
      gzipFound = FILE_IS_REAL(_file);
    }
  }

  bool found = fileFound || gzipFound;

  if (found)
  {
    _filename = path;

    // Calculate gzip statistic
    _gzipStats = (_gzipStats << 1) + (gzipFound ? 1 : 0);
    if (_gzipStats == 0x00) _gzipFirst = false; // All files are not gzip
    else if (_gzipStats == 0xFF) _gzipFirst = true; // All files are gzip
    else _gzipFirst = _countBits(_gzipStats) > 4; // IF we have more gzip files - try gzip first
  }

  return found;
}

uint8_t PsychicStaticFileHandler::_countBits(const uint8_t value) const
{
  uint8_t w = value;
  uint8_t n;
  for (n=0; w!=0; n++) w&=w-1;
  return n;
}

esp_err_t PsychicStaticFileHandler::handleRequest(PsychicRequest *request)
{
  if (_file == true)
  {
    DUMP(_filename);

    //is it not modified?
    String etag = String(_file.size());
    if (_last_modified.length() && _last_modified == request->header("If-Modified-Since"))
    {
      DUMP("Last Modified Hit");
      TRACE();
      _file.close();
      request->reply(304); // Not modified
    }
    //does our Etag match?
    else if (_cache_control.length() && request->hasHeader("If-None-Match") && request->header("If-None-Match").equals(etag))
    {
      DUMP("Etag Hit");
      DUMP(etag);
      DUMP(_cache_control);

      _file.close();

      PsychicResponse response(request);
      response.addHeader("Cache-Control", _cache_control.c_str());
      response.addHeader("ETag", etag.c_str());
      response.setCode(304);
      response.send();
    }
    //nope, send them the full file.
    else
    {
      DUMP("No cache hit");
      DUMP(_last_modified);
      DUMP(_cache_control);

      PsychicFileResponse response(request, _fs, _filename);

      if (_last_modified.length())
        response.addHeader("Last-Modified", _last_modified.c_str());
      if (_cache_control.length()) {
        response.addHeader("Cache-Control", _cache_control.c_str());
        response.addHeader("ETag", etag.c_str());
      }

      return response.send();
    }
  } else {
    return request->reply(404);
  }

  return ESP_OK;
}