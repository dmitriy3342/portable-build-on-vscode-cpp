# This Project Generator wrote by Dmitriy Angald from IT Space SU
# GitHub: dmitriy3342
# YouTube: https://www.youtube.com/channel/UCop5K-DDDJ-nKt6dn2UTgpg

import argparse
from distutils.dir_util import copy_tree
from distutils.file_util import copy_file
import os
import json
from typing import Dict, List

WORKSPACE_PATH = os.path.abspath('.')
VSCODE_PROJECT_PATH = os.path.join(WORKSPACE_PATH, '.vscode')
VSCODE_PROJECT_PATH_ESCAPED = VSCODE_PROJECT_PATH.replace("\\", "\\\\")
VSCODE_PATH = os.environ.get('VSCODE')
VSCODE_PATH_ESCAPED = VSCODE_PATH.replace("\\", "\\\\")
REPLACE_DICT = {
    '__VSCODE_PATH__': VSCODE_PATH_ESCAPED,
    '__BUILD_INPUT_FILE__': '${file}',
    '__BUILD_OUTPUT_FILE__': '${fileDirname}\\\\${fileBasenameNoExtension}.exe',
    '__BUILD_OUTPUT_DIR__': '${fileDirname}',
}


def print_dict(value: dict):
    print(json.dumps(value, indent=4, sort_keys=True))


def get_settings_list_by_name(type_project: str) -> List[Dict]:
    path = f'{VSCODE_PATH}\\data\\programs\\scripts\\templates\\settings\\{type_project}.json'
    print(f'Path to template: {path}')

    if not os.path.isfile(path):
        raise Exception(
            f'Not found tempate: "{type_project}"'
        )

    with open(path, 'r', encoding="utf-8") as f:
        data = f.read()
        return json.loads(data)


def create_project_by_template(type_project: str, template_name: str):
    print(f'Start of creating {type_project} project by template {template_name}')

    path = f'{VSCODE_PATH}\\data\\programs\\scripts\\templates\\\projects\\{type_project}\\{template_name}'
    copy_tree(path, WORKSPACE_PATH)

    path_readme = f'{VSCODE_PATH}\\data\\programs\\scripts\\README.MD'
    copy_file(path_readme, f'{WORKSPACE_PATH}\\README.MD')

    print(f'Created the Project')


def replace_values(text: str):
    for _from, to in REPLACE_DICT.items():
        text = text.replace(_from, to)
    return text


def add_configuration_for_current_file(current_file_path: str):
    current_file_path_escaped = current_file_path.replace('\\', '\\\\')
    current_file_path_splitted = current_file_path.split('\\')
    current_file_name = current_file_path_splitted[-1]
    current_file_name_without_extension = current_file_name.split('.')[0]
    current_dir_name_escaped = '\\\\'.join(current_file_path_splitted[:-1])

    REPLACE_DICT['__BUILD_INPUT_FILE__'] = f'{current_file_path_escaped}'
    REPLACE_DICT['__BUILD_OUTPUT_FILE__'] = f'{current_dir_name_escaped}\\\\{current_file_name_without_extension}.exe'
    REPLACE_DICT['__BUILD_OUTPUT_DIR__'] = f'{current_dir_name_escaped}'
    REPLACE_DICT['active file'] = f'{current_file_name_without_extension}'


def main():

    parser = argparse.ArgumentParser(description="Project Generator")
    parser.add_argument("--type_project", dest="type_project", required=True, action="store", help="Args for scripts: SFML")
    parser.add_argument("--current_file_path", dest="current_file_path", required=False, action="store", default='', help="Current file path")
    parser.add_argument("--template_name", dest="template_name", required=False, action="store", default='', help="Create project by template")

    args = parser.parse_args()

    type_project: str = args.type_project
    current_file_path: str = args.current_file_path
    template_name: str = args.template_name

    if template_name != '':
        current_file_path = ''
        create_project_by_template(type_project, template_name)
        try:
            with open(f'{WORKSPACE_PATH}\\main_file.txt', 'r', encoding='utf-8') as f:
                main_file_name = f.read().strip()
                current_file_path = f'{WORKSPACE_PATH}\\{main_file_name}'

            os.remove(f'{WORKSPACE_PATH}\\main_file.txt')
        except Exception as ex:
            print(ex)
            print(f'Incorrect main_file.txt for {template_name}')
            pass

    print(f'Start of creating Settings for: {type_project} project')

    if current_file_path != '':
        add_configuration_for_current_file(current_file_path)

    settings_list = get_settings_list_by_name(type_project)

    if not os.path.isdir(VSCODE_PROJECT_PATH):
        os.makedirs(VSCODE_PROJECT_PATH)

    if not os.path.isdir(VSCODE_PROJECT_PATH):
        os.makedirs(VSCODE_PROJECT_PATH)

    for settings in settings_list:
        file_name = settings['file_name']
        data = settings["data"]

        json_text = json.dumps(data, indent=4)
        json_text = replace_values(json_text)
        path = f'{VSCODE_PROJECT_PATH}\\{file_name}'
        with open(path, 'w', encoding="utf-8") as f:
            f.write(json_text)

        print(
            f'Created .vscode\\{file_name}\n'
            f'{json_text}'
        )


if __name__ == "__main__":
    main()
