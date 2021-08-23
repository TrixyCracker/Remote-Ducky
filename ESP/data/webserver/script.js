function init()
{
    let textarea = document.getElementById('script_area');                   
    textarea.addEventListener('keyup', countCharacters, false);
    countCharacters();
}

function send_script() {
    let input = document.getElementById("script_area");

    let script = input.value;

    if (script == "" || script == null)
    {
        alert("INSERISCI QUALCOSA, CAZZO!");
        return;
    }

    script = script.replace(/(\r\n|\n|\r)/gm, "\\n");

    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/update?script=" + script, true);
    xhr.send();
}

function countCharacters(e)
{
    let text = document.getElementById("script_area").value;

    let chars_number = text.length;

    let lines_number;
    try 
    {
        lines_number = text.match(/\n/gm).length;
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