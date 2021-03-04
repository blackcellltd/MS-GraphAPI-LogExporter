# Microsoft O365 Security Logs export

This tool exports data from dynamically configured Microsoft GraphQL endpoints, processes and stores them. Mainly focuses on retrieving security-related logs and events to make it possible to integrate Microsoft solutions into IBM Qradar or any SIEM in a Unix based environment.

---

## How does it work?

Let's say we want to retrieve Azure AD Sign-in events, we have filled the necessary authentication and resource identifiers into the config file, added the endpoint's information and started the tool. It has written out to us some startup information and slowly starts to create in organised manner files containing the latest events. We can just let it run and forget about it, it will do its job.

---

## What is under the hood?

Based on the default configuration it periodically makes at most two queries to every configured endpoint. The first labelled as a Normal query aims to retrieve the latest events. The Second one labelled as a Historical query focuses on those events that for some reason was missed by the Normal query, for example, rarely happens but some events only accessible houres after they happened. This query operates on a much wider time scope and slowly steps through it to make sure no events were left unnoticed.
The result of these queries go through a filter, which sorts out the duplicates, and the rest is being written out to a designated folder.

Sometimes because of rate control or other network connectivity issues, the querries might fall a bit behind, in this case after the issues are solved if the lag exceeds a critical limit, the Normal query switches into a Recovery mode. In this mode, it switches between catching up, with a bit bigger scope it querries the missed time frame, and slowly working down the fallback; and its normal querries, to still provide the latest events with low latency.

In some cases, when an event was created, it wasn't fully processed and some fields weren't filled yet, because of this shortly after it's creation the event was modified, unfortunately for us if we would like to reduce the latency as much as possible, we might retrieve an event between its creation and final modification, because of this by default we use a little delay reducing the probability of this little error to minimal.

The configuration is loaded and validated at the start of the tool if any critical error is identified the tool will stop its execution and an error message will be written to the standard output if everything is fine the tool starts its job and can be left unattended, the runtime log messages will be written out to the standard output.

---

## Installation

### Dependencies

- libcurl - https://curl.se/libcurl/
- jsoncpp - https://github.com/open-source-parsers/jsoncpp

### Compile locally

```
git clone CHANGE_THIS_FOR_THE_GIT_REPO's_URL
chmod +x ./MAKE.sh
./MAKE.sh build
```

---

## Configuration

All the configurations are loaded from a JSON file, by default from a file named `config.json` in the same directory as the executable, but it can be overwritten with a command-line parameter.

You can validate a config file with the `--validate-config [file name]` command-line argument or set the actual config file with the `--set-configFile <config file>` option.

### Required options

| Name                    | Type   | Discreptions                                                                                                                                                               |
| ----------------------- | ------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Access Intervals        | number | size of an access period in seconds                                                                                                                                        |
| Access Duration         | number | size of the scope that a Normal query uses in seconds                                                                                                                      |
| Access Delay            | number | size of the access delay in seconds                                                                                                                                        |
| Recovery Duration       | number | size of the scope that a Recovery query uses in seconds (it should be higher than the Access Duration)                                                                     |
| History Access Duration | number | length of the scope that a history query uses in seconds                                                                                                                   |
| History Duration        | number | length of the extended scope that a History query focuses on in seconds                                                                                                    |
| Log Path                | string | path of the designated directory of the exported log files                                                                                                                 |
| Log Level               | number | the highest runtime log level that should be shown, possible values: <br /><ol><li>Error</li><li>Success</li><li>Information</li><li>Debug</li><li>HTTP requests</li></ol> |
| Client Id               | string | Client Id for Microsoft OAuth2                                                                                                                                             |
| Client Secret           | string | Client Secret for Microsoft OAuth2                                                                                                                                         |
| Tenant Id               | string | Tenant Id for Microsoft OAuth2                                                                                                                                             |
| Api Base Url            | string | the base URL of the MsGraph API                                                                                                                                            |
| Endpoints               | array  | Array of the details of the endpoints                                                                                                                                      |

#### Endpoint configuration

| Name           | Type   | Discreptions                                       |
| -------------- | ------ | -------------------------------------------------- |
| Name           | string | Name of the endpoint                               |
| Endpoint path  | string | path of the endpoint                               |
| Timefield name | string | the key of the creation time value in the response |

---

## Usage

After the installation, modify the configuration file to include your credentials and add the endpoints you would like to access. Start the compiled executable file and let it run.
