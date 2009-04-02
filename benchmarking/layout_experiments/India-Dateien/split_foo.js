/* guard for onload */
var fired = 0;

/* an array of HTML for the sections */
var sections_html = new Array();

function logConsole(msg) {
    document.getElementById("console").innerHTML += "<span>" + msg + "</span><br />";
}

/*
 * Change the html...
 */
function switch_to(target) {
    document.getElementById("page").innerHTML = sections_html[target];    
}

/*
 * Create a push button... and hook it up
 * with javascript
 */
function createButton(caption, target) {
    button_panel = document.getElementById("buttons");
    button_html = '<button type="button" onclick="switch_to(' + target + ')">' +
            caption + '</button>';
    button_panel.innerHTML += button_html
}

function extractToc(toc, _content, sections) {
    createButton("TOC", sections_html.length);
    html = ""
    html += "<div>";
    html += toc[0].innerHTML;
    html += "<div>";

    for (i = 0; i < sections.length; ++i) {
        var h2_header = sections[i].previousSibling;
        if (!h2_header)
            continue;

        target = i + 1;
        html += '<a onclick="switch_to(' + target + ')" >' +
            h2_header.innerHTML + '</a><br />';
    }

    content = _content.cloneNode(true);
        
    html += "</div>";
    html += "</div>";
    sections_html[sections_html.length] = html;
}

function extractSections(sections) {
    var header = '<div>';

    for (section in sections) {
        var h2_header = sections[section].previousSibling;
        if (!h2_header)
            continue;

        /* create a button for it */
        createButton(h2_header.innerHTML, sections_html.length);

        /* create a html page for the iframe/div */
        html = header + h2_header.innerHTML + "<div>" +
                sections[section].innerHTML +
                "</div><script src='India-Dateien/cleanup.js'></script></div>";
        sections_html[sections_html.length] = html;
    }

    logConsole("Number of sections_html: " + sections_html.length);
}

function extractContent(iframe, sections) {
    /* Convert each section into a div/iframe */
    var sections = iframe.contentDocument.getElementsByClassName("content_block");
    var topic = iframe.contentDocument.getElementsByClassName("firstHeading");
    var bodyContent = iframe.contentDocument.getElementById("bodyContent");
    logConsole("Number of sections: " + sections.length);

    extractToc(topic, bodyContent, sections);
    extractSections(sections);
}
function start(iframe) {
    if (fired)
        return;

    fired = 1;
    /* start generating the view... we have everything loaded */
    try {
        extractContent(iframe);
    } catch(e) {
        logConsole(e);
    }
}

