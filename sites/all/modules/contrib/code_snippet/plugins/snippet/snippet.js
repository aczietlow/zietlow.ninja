(function ($) {
  Drupal.wysiwyg.plugins.snippet = {
    /**
     * Return whether the passed node belongs to this plugin.
     */
    isNode: function (node) {
      return $(node).is('img.code-snippet');
    },

    /**
     * Execute the button.
     */
    invoke: function (data, settings, instanceId) {

      settings.dialog.width = '850';
      settings.dialog.height = '480';
      settings.dialog.scrollbars = 'yes';

      if (data.format == 'html') {
        var options = {
          nid: Drupal.settings.codeSnippet.nid,
          type: '',
          csid: '',
          title: '',
          tagstyle: '',
          lineNumbering: '',
          numberStart: '',
          id: instanceId};
        if ($(data.node).is('img.code-snippet')) {
          // Expand inline tag in alt attribute
          data.node.alt = decodeURIComponent(data.node.alt);
          var chunks = data.node.alt.split('|');
          for (var i in chunks) {
            chunks[i].replace(/([^=]+)=(.*)/g, function(o, property, value) {
              options[property] = value;
            });
          }
          options.action = 'update';
        }
      }
      else {
        // @todo Plain text support.
      }
      
      if (typeof options != 'undefined') {
        Drupal.wysiwyg.instances[instanceId].openDialog(settings.dialog, options);
      }
    },

    /**
     * Replace inline tags in content with images.
     */
    attach: function (content, settings, instanceId) {
      content = content.replace(/\[snippet\|([^\[\]]+)\]/g, function(orig, match) {
        var node = {}, chunks = match.split('|');
        for (var i in chunks) {
          chunks[i].replace(/([^=]+)=(.*)/g, function(o, property, value) {
            node[property] = value;
          });
        }
        node.name = 'mceCodeSnippet';
        // 'class' is a predefined token in JavaScript.
        node['class'] = 'code-snippet';
        node.src = Drupal.settings.basePath + 'index.php?q=code-snippet/image/' + node.csid;
        node.alt = 'type=' + node.type + '|csid=' + node.csid + '|tagstyle=' + node.tagstyle
                 + '|lineNumbering=' + node.lineNumbering + '|numberStart=' + node.numberStart;
        node.alt = encodeURIComponent(node.alt);

        var element = '<img ';
        for (var property in node) {
          element += property + '="' + node[property] + '" ';
        }
        element += '/>';
        return element;
      });
      return content;
    },

    /**
     * Replace images with inline tags in content upon detaching editor.
     */
    detach: function (content, settings, instanceId) {
      var $content = $('<div>' + content + '</div>'); // No .outerHTML() in jQuery :(
      $('img', $content).each(function(node) {
        if (!$(this).is('.code-snippet')) {
          return;
        }
        var inlineTag = '[snippet|' + decodeURIComponent(this.alt) + ']';
        $(this).replaceWith(inlineTag);
      });
      return $content.html();
    }
  }
})(jQuery);
