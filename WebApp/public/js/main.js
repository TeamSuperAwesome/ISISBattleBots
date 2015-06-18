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

(function(){
	var CONTROLLER_URL = 'http://192.168.1.100';
	var MAX_SPEED = 200;
	var STEP = 25;
	var RATE_LIMIT_IN_SEC = 0.5;

	var controls = {};
	var lastSent;

	function canSend(){
		var canSend = false;
		if(!lastSent){
			canSend = true;
			lastSent = new Date();
		}else if(moment().isAfter(moment(lastSent).add(RATE_LIMIT_IN_SEC, 's'))){
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
				if(controls[command] < 0){
					controls[command] = 0;
				}
				if(controls[command] <= (MAX_SPEED - STEP)){
					controls[command] += STEP;
				}
			}else{
				if(controls[command] > 0){
					controls[command] = 0;
				}
				if(controls[command] >= (-MAX_SPEED + STEP)){
					controls[command] -= STEP;
				}
			}
			$.ajax({
			  type: "POST",
			  url: CONTROLLER_URL + "/" + $('#robotName').val().toLowerCase() + "/" + command + controls[command]
			});
		}
		var RATE_LIMIT_IN_SEC = 0.5;
	}

	// Clear any error state
	$('#robotName').on('change', function(){
		if($('#robotName').val().length){
			$('#robotName').parent().removeClass('has-error');
		}
	});

	// Events
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

	// Keyboard listeners
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

	// Use gyro as a control
	window.ondevicemotion = function(event) {
		var accelerationX, accelerationY;
		if(window.innerHeight > window.innerWidth){
			accelerationX = event.accelerationIncludingGravity.x;
			accelerationY = event.accelerationIncludingGravity.y;
		}else{
			accelerationX = -event.accelerationIncludingGravity.y;
			accelerationY = event.accelerationIncludingGravity.x;
		}

		if(accelerationY > 6){
			$.event.trigger({type:'battlebot:reverse'});
		}else if(accelerationY < 6){
			$.event.trigger({type:'battlebot:forward'});
		}
		if(accelerationX > 1.5){
			$.event.trigger({type:'battlebot:left'});
		}else if(accelerationX < -1.5){
			$.event.trigger({type:'battlebot:right'});
		}
	}


}());
