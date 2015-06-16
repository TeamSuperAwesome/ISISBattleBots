Array.prototype.remove = function() {
  var what, a = arguments, L = a.length, ax;
  while (L && this.length) {
    what = a[--L];
    while ((ax = this.indexOf(what)) !== -1) {
      this.splice(ax, 1);
    }
  }
  return this;
};

var controllerURL = '';
var MAX_SPEED = 200;
var controls = {};

function sendCommand(command, isOn){
	if(!controls[command]) controls[command] = 0;
	if(isOn){
		if(controls[command] <= (MAX_SPEED - 10)){
			controls[command] += 10;
		}
	}else{
		if(controls[command] >= 10){
			controls[command] -= 10;
		}
	}

	$.ajax({
	  type: "POST",
	  url: controllerURL,
	  data: {
	  	command: command,
	  	speed: controls[command],
	  	robot: $('#robotName').val()
	  },
	  success: function(){
	  	console.log("Sent");
	  }
	});
}

$(document).ready(function() {
	$('a.lead').removeClass('text-danger').addClass('text-success').find('span').text('Online');
	$('a.lead i.fa').removeClass('fa-times').addClass('fa-check');

	$(document).on('battlebot:forward', function(event){
		console.log('battlebot:forward')
		sendCommand('forward', false);
	});
	$(document).on('battlebot:reverse', function(event){
		console.log('battlebot:reverse')
		sendCommand('reverse', false);
	});


	$(document).on('battlebot:left', function(event){
		console.log('battlebot:left')
		sendCommand('direction', false);
	});
	$(document).on('battlebot:right', function(event){
		console.log('battlebot:right')
		sendCommand('direction', true);
	});

	$(document).on('battlebot:stop', function(event){
		console.log('battlebot:stop')
		sendCommand('stop', true);
	});

	$(document).on('battlebot:lights', function(event){
		console.log('battlebot:lights')
		if(event.prefix){
			sendCommand('lights', false);
		}else{
			sendCommand('lights', true);
		}
	});

	var keys = [];

  $(document).keydown(function(event) {
  	switch(event.which){
  		case 38:
  		case 87:
  			//forward
  			$('#btnForward').addClass('focus');
  			$.event.trigger({type:'battlebot:forward'});
  			break;
  		case 37:
  		case 65:
  			//left
  			$('#btnLeft').addClass('focus');
  			$.event.trigger({type:'battlebot:left'});
  			break;
  		case 39:
  		case 68:
  			//right
  			$('#btnRight').addClass('focus');
  			$.event.trigger({type:'battlebot:right'});
  			break;
			case 40:
			case 83:
  			//back
  			$('#btnReverse').addClass('focus');
  			$.event.trigger({type:'battlebot:reverse'});
  			break;
			default:
				break;
  	}
	});

	$(document).keyup(function(event){
		switch(event.which){
				case 38:
				case 87:
	  			//forward
	  			$('#btnForward').removeClass('focus');
	  			$.event.trigger({type:'battlebot:forward', prefix:'-'});
	  			break;
	  		case 37:
	  		case 65:
	  			//left
	  			$('#btnLeft').removeClass('focus');
	  			$.event.trigger({type:'battlebot:left', prefix:'-'});
	  			break;
	  		case 39:
	  		case 68:
	  			//right
	  			$('#btnRight').removeClass('focus');
	  			$.event.trigger({type:'battlebot:right', prefix:'-'});
	  			break;
				case 40:
				case 83:
	  			//back
	  			$('#btnReverse').removeClass('focus');
	  			$.event.trigger({type:'battlebot:reverse', prefix:'-'});
	  			break;
				default:
					break;
	  	}
	});
});