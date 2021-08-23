function init() {
    let textarea = document.getElementById('script_area');
    textarea.addEventListener('keyup', countCharacters, false);
    countCharacters();
}

function send_script() {
    let input = document.getElementById("script_area");

    let script = input.value;

    if (script == "" || script == null) {
        alert("INSERISCI QUALCOSA, CAZZO!");
        return;
    }

    script = script.replace(/(\r\n|\n|\r)/gm, "\\n");

    let caps_check = document.getElementById("caps_check");

    if (caps_check.checked) {
        let strings = script.split("\\n");

        let str = "";

        strings.forEach(element => {
           let first_space = element.indexOf(' ');

           if (first_space != -1)
           {
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
           else
           {
               str += element;
           }

           str += "\\n";
        });

        script = str;
    }

    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?script=" + script, true);
    xhr.send();
}

function countCharacters(e) {
    let text = document.getElementById("script_area").value;

    let chars_number = text.length;

    let lines_number;
    try {
        lines_number = text.match(/\n/gm).length + 1;
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