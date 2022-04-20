let vpp_units = document.querySelector("#Vpp");

function update_vpp(event) {

fetch("/vpp_units").then((response) => {
return response.text();
}).then((data) => {
let vpp_val = parseInt(data);
if (event.target.value === "Vpp") {
vpp_val = vpp_val / 1000;
}
vpp_units.innerHTML = `Vpeak-peak: <span>${vpp_val} </span>`;
});
}

let vp_units = document.querySelector("#Vp");

function update_vp(event) {

fetch("/vp_units").then((response) => {
return response.text();
}).then((data) => {
let vp_val = parseInt(data);
if (event.target.value === "Vp") {
vp_val = vp_val / 1000;
}
vp_units.innerHTML = `Vpeak: <span>${vp_val} </span>`;
});
}

let freq_units = document.querySelector("#Freq");

function update_freq(event) {

fetch("/freq_units").then((response) => {
return response.text();
}).then((data) => {
let freq_val = parseInt(data);
if (event.target.value === "kHz") {
freq_val = freq_val / 1000;
}
freq_units.innerHTML = `Frequency: <span>${freq_val} </span>`;
});
}

let period_units = document.querySelector("#Period");

function update_period(event) {

fetch("/period_units").then((response) => {
return response.text();
}).then((data) => {
let period_val = parseInt(data);
if (event.target.value === "s") {
period_val = period_val / 1000;
}
period_units.innerHTML = `Period: <span>${period_val} </span>`;
});
}