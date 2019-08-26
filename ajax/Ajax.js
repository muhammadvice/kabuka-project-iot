
function FungsiAjax(formData, url) {

	$.ajax({
		url: url,
		type: "GET",
		beforeSend: function (xhr) {

		},
		success: function (data) {

			var t_data = "";
			$.each(data, function(index, value) {
				$.each(value, function(index2, value2) {
					t_data += "<tr>";
					t_data += "<td>" + value2.time + "</td>";
					t_data += "<td>" + value2.state + "</td>";
					t_data += "</tr>";
				});
			});
			$("#statetable tbody").html(t_data);
		},
		error: function () {
			// $('button').attr('disabled', false);

			// toastr.error('Cek Hubungan Internet Anda atau Terjadi kesalahan yang tidak diketahui penyebabnya', "Woahh!");

			// setTimeout(
			// 	function () {
			// 		loading.hide();
			// 		//do something special
			// 	}, 1000);
			return false;
		}
	});
}
