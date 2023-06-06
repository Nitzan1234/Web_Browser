#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <curl/curl.h>

// Go button clicked event handler
void goButtonClicked(GtkButton *button, GtkEntry *entry, WebKitWebView *webView) {
    const gchar *url = gtk_entry_get_text(entry);

    // Create a CURL handle
    CURL *curl = curl_easy_init();
    if (curl) {
        // Set the URL to be requested
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the write callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, webviewWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, webView);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            // Handle request error
            g_print("Failed to perform request: %s\n", curl_easy_strerror(res));
        }

        // Cleanup the CURL handle
        curl_easy_cleanup(curl);
    }
}

// CURL write callback function to handle the response
size_t webviewWriteCallback(char *data, size_t size, size_t nmemb, WebKitWebView *webView) {
    size_t totalSize = size * nmemb;

    // Convert the response data to a GBytes object
    GBytes *responseBytes = g_bytes_new(data, totalSize);

    // Load the response data into the web view
    webkit_web_view_load_bytes(webView, responseBytes, NULL);

    // Free the GBytes object
    g_bytes_unref(responseBytes);

    return totalSize;
}

void webViewNavigation(WebKitWebView *webView, WebKitLoadEvent event, gpointer data) {
    if (event == WEBKIT_LOAD_FINISHED) {
        // Get the HTML content of the loaded page
        WebKitWebResource *mainResource = webkit_web_view_get_main_resource(webView);
        if (mainResource) {
            GInputStream *inputStream = webkit_web_resource_get_data(mainResource);
            if (inputStream) {
                GDataInputStream *dataInputStream = g_data_input_stream_new(inputStream);
                if (dataInputStream) {
                    // Read the HTML content
                    GError *error = NULL;
                    gchar *htmlContent = g_data_input_stream_read_until(dataInputStream, "</html>", -1, NULL, &error);
                    if (htmlContent) {
                        // Parse the HTML content to extract JavaScript code
                        const gchar *scriptStartTag = "<script>";
                        const gchar *scriptEndTag = "</script>";
                        gchar *scriptStartPtr = strstr(htmlContent, scriptStartTag);
                        while (scriptStartPtr) {
                            scriptStartPtr += strlen(scriptStartTag);
                            gchar *scriptEndPtr = strstr(scriptStartPtr, scriptEndTag);
                            if (scriptEndPtr) {
                                // Extract JavaScript code between script tags
                                gchar *jsCode = g_strndup(scriptStartPtr, scriptEndPtr - scriptStartPtr);
                                if (jsCode) {
                                    // Execute the JavaScript code
                                    printf("Executing JavaScript code: %s\n", jsCode);
                                    executeJavaScript(jsCode);
                                    g_free(jsCode);
                                }
                                scriptStartPtr = strstr(scriptEndPtr, scriptStartTag);
                            } else {
                                break;
                            }
                        }
                        g_free(htmlContent);
                    } else {
                        fprintf(stderr, "Error reading HTML content: %s\n", error->message);
                        g_error_free(error);
                    }
                    g_object_unref(dataInputStream);
                }
                g_object_unref(inputStream);
            }
        }
    }
}

// CURL write callback function to handle the response
size_t webviewWriteCallback(char *data, size_t size, size_t nmemb, WebKitWebView *webView) {
    size_t totalSize = size * nmemb;

    // Convert the response data to a GBytes object
    GBytes *responseBytes = g_bytes_new(data, totalSize);

    // Load the response data into the web view
    webkit_web_view_load_bytes(webView, responseBytes, NULL);

    // Free the GBytes object
    g_bytes_unref(responseBytes);

    // Parse and process the HTML response
    parseHTMLResponse(data, totalSize);

    return totalSize;
}

// HTML parsing and processing function
void parseHTMLResponse(char *htmlData, size_t dataSize) {
    // Parse the HTML document
    htmlDocPtr doc = htmlReadMemory(htmlData, dataSize, NULL, NULL, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
    if (doc == NULL) {
        printf("Failed to parse HTML.\n");
        return;
    }

    // Get the root element of the HTML document
    xmlNode *rootElement = xmlDocGetRootElement(doc);

    // Traverse and process the parsed HTML tree (you can customize this function according to your needs)
    processHTMLElements(rootElement);

    // Cleanup the parsed HTML document
    xmlFreeDoc(doc);
}

// Process HTML elements function (customize this function according to your needs)
void processHTMLElements(xmlNode *node) {
    xmlNode *curNode = NULL;
    for (curNode = node; curNode; curNode = curNode->next) {
        if (curNode->type == XML_ELEMENT_NODE) {
            // Process the HTML element (you can customize this part)
            printf("Element: %s\n", curNode->name);
        }
        processHTMLElements(curNode->children);
    }
}

