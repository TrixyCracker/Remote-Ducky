function init() {
    let textarea = document.getElementById('script_area');
    textarea.addEventListener('keyup', countCharacters, false);
    countCharacters();
    setup_scriptlist();
}

function setup_scriptlist() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let response = this.responseText;

            if (response == "")
            {
                script_list.innerHTML = "None";
                return;
            }

            let response_splitted = response.split("-");

            let script_list = document.getElementById("script_list");
            script_list.innerHTML = "";

            response_splitted.forEach(scriptname => {
                let a = document.createElement("a");
                a.setAttribute("class", "min_button inline");
                a.setAttribute("onClick", "read_script(\"" + scriptname + "\")");

                let text = document.createTextNode(scriptname);

                a.appendChild(text);

                script_list.appendChild(a);
            });
        }
    };
    xhttp.open("GET", "/scriptlist", true);
    xhttp.send();
}

function send_script() {
    let input = document.getElementById("script_area");

    let script = input.value;

    if (script == "" || script == null) {
        alert("INSERISCI QUALCOSA, CAZZO!");
        return;
    }

    script = script.replace(/(\r\n|\n|\r)/g, "@@");

    let caps_check = document.getElementById("caps_check");

    if (caps_check.checked) {
        let strings = script.split("@@");

        let str = "";

        strings.forEach(element => {
            let first_space = element.indexOf(' ');

            if (first_space != -1) {
                let part1 = element.substring(0, first_space + 1);
                let part2 = element.substring(first_space + 1);

                str += part1;

                for (let i = 0; i < part2.length; ++i) {
                    if (part2[i] == part2[i].toLowerCase()) {
                        str += part2[i].toUpperCase();
                    }
                    else {
                        str += part2[i].toLowerCase();
                    }
                }
            }
            else {
                str += element;
            }

            str += "@@";
        });

        script = str;
    }

    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?run=" + script, true);
    xhr.send();
}

function save_script() {
    let input = document.getElementById("script_area");

    let script_code = input.value;

    if (script_code == "" || script_code == null) {
        alert("INSERISCI QUALCOSA, CAZZO!");
        return;
    }

    script_code = script_code.replace(/(\r\n|\n|\r)/g, "@@");

    let script_name = prompt("Inserisci il nome dello script");

    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?name=" + script_name + "&script=" + script_code, true);
    xhr.send();
}

function read_script(name) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let response = this.responseText;

            response = response.replace(/\@{2}/g, "\n");

            let script_area = document.getElementById("script_area");
            script_area.value = response;
        }
    };
    xhttp.open("GET", "/scriptread?name=" + name, true);
    xhttp.send();
}

function download_script() {
    let script = document.getElementById("script_area").value;

    let filename = prompt("Insert filename:");

    if (script == "" || filename == "") {
        alert("INSERISCI QUALCOSA, CAZZO!");
        return;
    }

    filename += ".txt";

    var downloader = document.createElement('a');
    downloader.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(script));
    downloader.setAttribute('download', filename);

    downloader.style.display = 'none';
    document.body.appendChild(downloader);

    downloader.click();

    document.body.removeChild(downloader);
}

function countCharacters(e) {
    let text = document.getElementById("script_area").value;

    let chars_number = text.length;

    let lines_number;
    try {
        lines_number = text.match(/\n/g).length + 1;
    }
    catch
    {
        lines_number = 1;
    }

    let char_counter = document.getElementById("char_counter");
    let line_counter = document.getElementById("line_counter");

    char_counter.innerText = "Characters: " + chars_number;
    line_counter.innerText = "Lines: " + lines_number;
}