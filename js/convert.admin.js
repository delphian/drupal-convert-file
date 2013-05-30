(function($) {
  Drupal.behaviors.mtrvListingHelper = {
    attach: function (context) {
      this.me = Drupal.behaviors.mtrvListingHelper;
      $("#edit-instance-widget-settings-convertfile-provider").bind('change', function() {
        // get value
        var value = $(this).val(); 
        Drupal.behaviors.mtrvListingHelper.limitOptions(value);  
      });

      // hide values that do not currently apply
      // we have to hid any value that ISN'T under this provider
      // that would be fine if we knew which option was selected
      // but for sack of cleaniness, we are going to not use limitOptions
      var currentValue = $("#edit-instance-widget-settings-convertfile-provider").val();
      for(var key in convertfile) {
        if (key != currentValue) {
          for (var format in convertfile[key].types) {
            $("option[value='" + format + "']").remove();
          }
        }
      }
    },

    limitOptions: function(value) {
      var selectBox = $("#edit-instance-widget-settings-convertfile-format");
      selectBox.children("option").remove();
      selectBox.append('<option value="none">- None -</option>');
      // change values
      for(var key in convertfile[value].types) {
        var value = convertfile[value].types[key];
        selectBox
          .append($("<option></option>")
          .attr("value",key)
          .text(value)); 
      }
    }
  }
})(jQuery);

