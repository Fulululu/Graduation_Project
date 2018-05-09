function update(){
	$.getJSON('/update/', function(jsondata){
		$.each(jsondata, function(){
			$('#showdata').html('<td scope="col">'+this.UID_id+'</td>'
						    + '<td>'+this.NODE+'</td>'
						    + '<td>'+this.LIGHT+'</td>'
						    + '<td>'+this.AIRTEMP+'</td>'
						    + '<td>'+this.AIRHUMI+'</td>'
						    + '<td>'+this.SOILTEMP+'</td>'
						    + '<td>'+this.SOILHUMI+'</td>');  
			});	
		});
	}
function timeDown(limit, i){  
	limit--;  
	if (i > 60){  
		i = 0;  
	}  
	if (limit < 0){  
		limit = 60;  
		update(i);  
		i++;  
	}  
	//$('#time').text(limit + '秒后刷新');  
	setTimeout(function(){
		timeDown(limit, i);
		}, 1000)  
	}  


$(document).ready(function(){
	/*
	 * Start update real time data 
	 */
	timeDown(60, 0) 
	/*
	 * 以下代码控制按钮切换效果
 	 */
	$('.btn-group').click(function(){
	    	var $this = $(this);

	    	if($this.find('.btn-warning').length>0){
	      		$this.find('.btn').toggleClass('btn-warning');
	    	}

	    	if($this.find('.btn-primary').length>0){
	      		$this.find('.btn').toggleClass('btn-primary');
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
