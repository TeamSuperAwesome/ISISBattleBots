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
var STEP = 25;
var controls = {};

var lastSent;

function canSend(){
	var canSend = false;
	if(!lastSent){
		canSend = true;
		lastSent = new Date();
	}else if(moment().isAfter(moment(lastSent).add(0.5, 's'))){
		canSend = true;
		lastSent = new Date();
	}
	return canSend;
}

function sendCommand(command, isOn){
	if(!$('#robotName').val().length){
		$('#robotName').parent().addClass('has-error');
		return;
	}
	$('#robotName').parent().removeClass('has-error');
	if(canSend()){
		if(!controls[command]) controls[command] = 0;
		if(isOn){
			if(controls[command] <= (MAX_SPEED - STEP)){
				controls[command] += STEP;
			}
		}else{
			if(controls[command] >= (-MAX_SPEED + STEP)){
				controls[command] -= STEP;
			}
		}
		$.ajax({
		  type: "POST",
		  url: controllerURL + "/" + $('#robotName').val() + "/" + command + controls[command]
		});
	}
}

$(document).ready(function() {
	$('a.lead').removeClass('text-danger').addClass('text-success').find('span').text('Online');
	$('a.lead i.fa').removeClass('fa-times').addClass('fa-check');

	$('#robotName').on('change', function(){
		if($('#robotName').val().length){
			$('#robotName').parent().removeClass('has-error');
		}
	});


	$(document).on('battlebot:forward', function(event){
		sendCommand('M', true);
	});
	$(document).on('battlebot:reverse', function(event){
		sendCommand('M', false);
	});


	$(document).on('battlebot:left', function(event){
		sendCommand('T', false);
	});
	$(document).on('battlebot:right', function(event){
		sendCommand('T', true);
	});

	$(document).on('battlebot:stop', function(event){
		sendCommand('S', true);
	});

	$(document).on('battlebot:lights', function(event){
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
		case 32:
  			//stop
  			$('#btnStop').addClass('focus');
  			$.event.trigger({type:'battlebot:stop'});
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
			case 32:
	  			//stop
	  			$('#btnStop').removeClass('focus');
	  			$.event.trigger({type:'battlebot:stop', prefix:'-'});
	  			break;
			default:
				break;
	  	}
	});
});