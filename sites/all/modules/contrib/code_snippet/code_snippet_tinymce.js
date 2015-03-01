
//Declare global variables
var nid, csid, myTextarea, type, tagstyle, lineNumbering, numberStart;

function initLoader(redirect) {
  nid         =      tinyMCEPopup.getWindowArg('nid');

  if (nid == null) {
    nid = 0;
  }

  type        =      tinyMCEPopup.getWindowArg('type');
  csid        = '' + tinyMCEPopup.getWindowArg('csid');
  myTextarea  = '' + tinyMCEPopup.getWindowArg('id');
  tagstyle    = '' + tinyMCEPopup.getWindowArg('tagstyle');
  lineNumbering = '' + tinyMCEPopup.getWindowArg('lineNumbering');
  numberStart   = '' + tinyMCEPopup.getWindowArg('numberStart');

  csid = csid.length ? csid : 0;
  var extras = '&tagstyle=' + tagstyle + '&lineNumbering=' + lineNumbering + '&numberStart=' + numberStart;
  if (redirect) {
    window.location.href = Drupal.settings.basePath + 'index.php?q=code-snippet/tinymce/' + myTextarea + '/' + nid + '/' + csid + extras;
  }
}

function insertToEditor(content) {
  tinyMCEPopup.editor.execCommand('mceInsertContent', false, content);
  return cancelAction();
}

function cancelAction() {
  tinyMCEPopup.close();
  return false;
}