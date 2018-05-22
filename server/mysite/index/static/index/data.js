function update(opt){
	if(opt == 0){
		$.getJSON('/update/'+opt, function(jsondata){
			$.each(jsondata, function(){
				$('#'+this.NODE).html('<td scope="col">'+this.UID_id+'</td>'
							    + '<td>'+this.NODE+'</td>'
							    + '<td>'+this.LIGHT+'</td>'
							    + '<td>'+this.AIRTEMP+'</td>'
							    + '<td>'+this.AIRHUMI+'</td>'
							    + '<td>'+this.SOILTEMP+'</td>'
							    + '<td>'+this.SOILHUMI+'</td>');
				});	
			});
	}else if(opt == 1){
		$.getJSON('/update/'+opt, function(jsondata){
			$.each(jsondata, function(){
				if(this.pump_state){
					obj = document.getElementById("pump_on");
					obj.className='btn btn-lg btn-primary';
					obj = document.getElementById("pump_off");
					obj.className='btn btn-lg btn-default';
				}else{
					obj = document.getElementById("pump_on");
					obj.className='btn btn-lg btn-default';
					obj = document.getElementById("pump_off");
					obj.className='btn btn-lg btn-primary';				
				}	
				if(this.lamp_state){
					obj = document.getElementById("lamp_on");
					obj.className='btn btn-lg btn-warning';
					obj = document.getElementById("lamp_off");
					obj.className='btn btn-lg btn-default';
				}else{
					obj = document.getElementById("lamp_on");
					obj.className='btn btn-lg btn-default';
					obj = document.getElementById("lamp_off");
					obj.className='btn btn-lg btn-warning';
				}
				});	
			});
	}
}
//backup must be 0
function timeDown(time, backup, opt){  	
	time--;
	if (time < 0){
		time = backup;
		backup = 0;
		update(opt);
		if(opt==0) opt=1;
  		else if(opt==1) opt=0;
	}else{
		backup++;
	}
	//$('#time').text(time + '秒后刷新');  
	setTimeout(function(){
		timeDown(time, backup, opt);
		}, 1000)  
	}  
//dev_id = 0 is PUMP, 1 is LAMP, 2 update device_state.
function devctl(dev_id){
	$.getJSON('/devctl/'+dev_id, function(jsondata){
		$.each(jsondata, function(){
			if(dev_id == 0){
				//$('#content').html('<p>PUMP Status:'+this.pump_state+'</p>');  
			}
			else if(dev_id == 1){
				//$('#content').html('<p>LAMP Status:'+this.lamp_state+'</p>');
			}
		});	
	});
}

$(document).ready(function(){
	/*
	 * Start update real time data and device states for device button 
	 */
	timeDown(3, 0, 0)
	/*
	 * 以下代码控制按钮切换效果
 	 */
	$('.btn-group').click(function(){
	    	var $this = $(this);

	    	if($this.find('.btn-primary').length>0){
	      		$this.find('.btn').toggleClass('btn-primary');
			devctl(0);
	    	}

	    	if($this.find('.btn-warning').length>0){
	      		$this.find('.btn').toggleClass('btn-warning');
			devctl(1);
	    	}

	});
	/*
	 * 以下代码控制导航键切换效果
 	 */
	$('#v-pills-present-data-tab').click(function(){
		if($('#v-pills-all-data-tab').find('.active')){
			$('#v-pills-all-data-tab').removeClass('active');
		}
		if($('#v-pills-light-tab').find('.active')){
			$('#v-pills-light-tab').removeClass('active');
		}
		if($('#v-pills-air-tempture-tab').find('.active')){
			$('#v-pills-air-tempture-tab').removeClass('active');
		}
		if($('#v-pills-air-humidity-tab').find('.active')){
			$('#v-pills-air-humidity-tab').removeClass('active');
		}
		if($('#v-pills-soil-tempture-tab').find('.active')){
			$('#v-pills-soil-tempture-tab').removeClass('active');
		}
		if($('#v-pills-soil-humidity-tab').find('.active')){
			$('#v-pills-soil-humidity-tab').removeClass('active');
		}	      	
	});

	$('#v-pills-device-manage-tab').click(function(){
		if($('#v-pills-all-data-tab').find('.active')){
			$('#v-pills-all-data-tab').removeClass('active');
		}
		if($('#v-pills-light-tab').find('.active')){
			$('#v-pills-light-tab').removeClass('active');
		}
		if($('#v-pills-air-tempture-tab').find('.active')){
			$('#v-pills-air-tempture-tab').removeClass('active');
		}
		if($('#v-pills-air-humidity-tab').find('.active')){
			$('#v-pills-air-humidity-tab').removeClass('active');
		}
		if($('#v-pills-soil-tempture-tab').find('.active')){
			$('#v-pills-soil-tempture-tab').removeClass('active');
		}
		if($('#v-pills-soil-humidity-tab').find('.active')){
			$('#v-pills-soil-humidity-tab').removeClass('active');
		}	      	
	});

	$('#v-pills-about-system-tab').click(function(){
		if($('#v-pills-all-data-tab').find('.active')){
			$('#v-pills-all-data-tab').removeClass('active');
		}
		if($('#v-pills-light-tab').find('.active')){
			$('#v-pills-light-tab').removeClass('active');
		}
		if($('#v-pills-air-tempture-tab').find('.active')){
			$('#v-pills-air-tempture-tab').removeClass('active');
		}
		if($('#v-pills-air-humidity-tab').find('.active')){
			$('#v-pills-air-humidity-tab').removeClass('active');
		}
		if($('#v-pills-soil-tempture-tab').find('.active')){
			$('#v-pills-soil-tempture-tab').removeClass('active');
		}
		if($('#v-pills-soil-humidity-tab').find('.active')){
			$('#v-pills-soil-humidity-tab').removeClass('active');
		}	      	
	});


	$('#v-pills-all-data-tab').click(function(){
		if($('#v-pills-present-data-tab').find('.active')){
			$('#v-pills-present-data-tab').addClass('mynavcolor');
		      	$('#v-pills-present-data-tab').removeClass('active');
		}
		if($('#v-pills-device-manage-tab').find('.active')){
			$('#v-pills-device-manage-tab').addClass('mynavcolor');
		      	$('#v-pills-device-manage-tab').removeClass('active');
		}
		if($('#v-pills-about-system-tab').find('.active')){
			$('#v-pills-about-system-tab').addClass('mynavcolor');
		      	$('#v-pills-about-system-tab').removeClass('active');
		}
	});

	$('#v-pills-light-tab').click(function(){
		if($('#v-pills-present-data-tab').find('.active')){
			$('#v-pills-present-data-tab').addClass('mynavcolor');
		      	$('#v-pills-present-data-tab').removeClass('active');
		}
		if($('#v-pills-device-manage-tab').find('.active')){
			$('#v-pills-device-manage-tab').addClass('mynavcolor');
		      	$('#v-pills-device-manage-tab').removeClass('active');
		}
		if($('#v-pills-about-system-tab').find('.active')){
			$('#v-pills-about-system-tab').addClass('mynavcolor');
		      	$('#v-pills-about-system-tab').removeClass('active');
		}	
	});

	$('#v-pills-air-tempture-tab').click(function(){	
		if($('#v-pills-present-data-tab').find('.active')){
			$('#v-pills-present-data-tab').addClass('mynavcolor');
		      	$('#v-pills-present-data-tab').removeClass('active');
		}
		if($('#v-pills-device-manage-tab').find('.active')){
			$('#v-pills-device-manage-tab').addClass('mynavcolor');
		      	$('#v-pills-device-manage-tab').removeClass('active');
		}
		if($('#v-pills-about-system-tab').find('.active')){
			$('#v-pills-about-system-tab').addClass('mynavcolor');
		      	$('#v-pills-about-system-tab').removeClass('active');
		}
	});

	$('#v-pills-air-humidity-tab').click(function(){	
		if($('#v-pills-present-data-tab').find('.active')){
			$('#v-pills-present-data-tab').addClass('mynavcolor');
		      	$('#v-pills-present-data-tab').removeClass('active');
		}
		if($('#v-pills-device-manage-tab').find('.active')){
			$('#v-pills-device-manage-tab').addClass('mynavcolor');
		      	$('#v-pills-device-manage-tab').removeClass('active');
		}
		if($('#v-pills-about-system-tab').find('.active')){
			$('#v-pills-about-system-tab').addClass('mynavcolor');
		      	$('#v-pills-about-system-tab').removeClass('active');
		}
	});

	$('#v-pills-soil-tempture-tab').click(function(){	
		if($('#v-pills-present-data-tab').find('.active')){
			$('#v-pills-present-data-tab').addClass('mynavcolor');
		      	$('#v-pills-present-data-tab').removeClass('active');
		}
		if($('#v-pills-device-manage-tab').find('.active')){
			$('#v-pills-device-manage-tab').addClass('mynavcolor');
		      	$('#v-pills-device-manage-tab').removeClass('active');
		}
		if($('#v-pills-about-system-tab').find('.active')){
			$('#v-pills-about-system-tab').addClass('mynavcolor');
		      	$('#v-pills-about-system-tab').removeClass('active');
		}
	});

	$('#v-pills-soil-humidity-tab').click(function(){	
		if($('#v-pills-present-data-tab').find('.active')){
			$('#v-pills-present-data-tab').addClass('mynavcolor');
		      	$('#v-pills-present-data-tab').removeClass('active');
		}
		if($('#v-pills-device-manage-tab').find('.active')){
			$('#v-pills-device-manage-tab').addClass('mynavcolor');
		      	$('#v-pills-device-manage-tab').removeClass('active');
		}
		if($('#v-pills-about-system-tab').find('.active')){
			$('#v-pills-about-system-tab').addClass('mynavcolor');
		      	$('#v-pills-about-system-tab').removeClass('active');
		}
	});
});
