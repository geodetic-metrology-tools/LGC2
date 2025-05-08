# JSON Structure Documentation

This document describes the structure of the JSON file and the purpose of each section. For each part of the JSON, an example is provided along with a table summarizing the field descriptions, formats, and units where applicable.

---

## Main Structure

The top-level JSON contains metadata and the main `LGC_DATA` block. This block holds the essential data used in the calculations.

```json
{
  "fCopyright": "Copyright 2022, CERN SU. All rights reserved.",
  "LGCVersion": "v2.8.0",
  "startProcessingTimestamp": "2023-10-25T23:23:56+0200",
  "processingElapsedSeconds": 4.3585863,
  "LGC_DATA": {}
}
```

### Field Descriptions

| Field                      | Description                                                    | Format/Unit |
|----------------------------|----------------------------------------------------------------|-------------|
| `fCopyright`               | LGC copyright declaration.                                     | String      |
| `LGCVersion`               | Version of LGC that produced the current file.                 | String      |
| `startProcessingTimestamp` | Timestamp when the process started.                            | ISO 8601    |
| `processingElapsedSeconds` | Duration the process lasted.                                   | Seconds     |
| `LGC_DATA`                 | Main container for all calculation-related data.               | Object      |

---

## LGC_DATA Structure

The `LGC_DATA` object stores various fields concerning the calculation. It includes elements such as geometric parameters, configuration settings, statistical data, and more.

```json
"LGC_DATA": {
  "angles": [],
  "config": {},
  "fLSRelatedInfo": {},
  "fMeasInfo": {},
  "fPointInfo": {},
  "fRelErrors": {},
  "fUEOIndices": {},
  "instruments": {},
  "islgc1": false,
  "lengths": [],
  "lines": [],
  "planes": [],
  "points": [],
  "stat": {},
  "tree": [],
  "slaveGroups": [],
  "pointConstraintGroups": []
}
```

| Field                     | Description                                                                                          |
|---------------------------|------------------------------------------------------------------------------------------------------|
| `angles`                  | List of adjustable angles.                                                                           |
| `config`                  | Configuration parameters from the input file.                                                        |
| `fLSRelatedInfo`          | Information related to the least squares (LS) adjustment process.                                    |
| `fMeasInfo`               | Measurement counts for each observation type.                                                        |
| `fPointInfo`              | Point counts corresponding to each constraint type.                                                  |
| `fRelErrors`              | Lists of relative errors between points (`*EREL`) and between frames (`*ERELFRAME`).                  |
| `fUEOIndices`             | Counts of unknowns, equations, observations, and constraints.                                        |
| `instruments`             | Metadata about instruments used for data acquisition.                                                |
| `islgc1`                  | Boolean flag: `true` if the file follows the LGC1 standard; `false` if it follows LGC2.                |
| `lengths`, `lines`, `planes`, `points` | Lists of adjustable geometric elements (lengths, lines, planes, points).                   |
| `stat`                    | Statistical information from the LS adjustment process.                                              |
| `tree`                    | Frame tree details including transformation parameters and associated observations.                  |
| `slaveGroups`             | List of slave groups.                                                                                |
| `pointConstraintGroups`   | List of point constraint groups.                                                                     |


---

## Additional Formatting Notes

- **Syntax Highlighting:**  
  The JSON examples are marked with a `json` code block tag for proper syntax highlighting.

- **Expandable Sections:**  
  The `<details>` tag is used to collapse or expand detailed field descriptions, keeping the document clean and easy to navigate.

- **Tables:**  
  Tables are used to clearly summarize the key aspects of each field along with descriptions, formats, and units where relevant.

---

Feel free to adjust any descriptions or expand sections as your documentation evolves. This approach should help users quickly grasp the JSON structure while allowing them to dive into details as needed.
