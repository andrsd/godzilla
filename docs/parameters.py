from docutils import nodes
from docutils.parsers.rst import Directive, directives
import yaml
import os

class ParametersDirective(Directive):
    has_content = True
    required_arguments = 1

    def run(self):
        config = self.state.document.settings.env.config
        file_path = config.parameters_yaml_file
        class_name = self.arguments[0]

        if not os.path.isfile(file_path):
            error = self.state_machine.reporter.error(
                f'File not found: {file_path}', nodes.literal_block(self.block_text, self.block_text), line=self.lineno)
            return [error]

        with open(file_path, 'r') as file:
            data = yaml.safe_load(file)

        class_data = next((cls for cls in data['classes'] if cls['name'] == class_name), None)
        if class_data is None:
            error = self.state_machine.reporter.error(
                f'Class {class_name} not found in {file_path}', nodes.literal_block(self.block_text, self.block_text), line=self.lineno)
            return [error]

        parameters = class_data['parameters']
        required = [param for param in parameters if param['required'] == 1]
        optional = [param for param in parameters if param['required'] == 0]

        # section = nodes.section()
        # section['ids'].append('parameters-' + class_name.lower().replace(' ', '-'))

        param_list = nodes.bullet_list()

        if required:
            # section += nodes.Text('Required')
            # section['ids'].append('parameters-' + class_name.lower().replace(' ', '-'))
            it = nodes.list_item('', nodes.strong(text='Required:'))
            bullet_list = nodes.bullet_list()
            for param in required:
                item = nodes.list_item()
                para = nodes.paragraph()
                para += nodes.strong(text=f"{param['name']} ({param['type']}) - ")
                para += nodes.Text(param['description'])
                item += para
                bullet_list += item
            it += bullet_list
            param_list += it
        if optional:
            it = nodes.list_item('', nodes.strong(text='Optional:'))
            # section += nodes.Text('Optional')
            # section['ids'].append('parameters-' + class_name.lower().replace(' ', '-'))
            bullet_list = nodes.bullet_list()
            for param in optional:
                item = nodes.list_item()
                para = nodes.paragraph()
                para += nodes.strong(text=f"{param['name']} ({param['type']}) - ")
                para += nodes.Text(param['description'])
                item += para
                bullet_list += item
            it += bullet_list
            param_list += it

        return [param_list]

def setup(app):
    app.add_directive('parameters', ParametersDirective)
    app.add_config_value('parameters_yaml_file', '', 'env')
