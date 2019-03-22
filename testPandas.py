# Import pandas
import pandas as pd

pd.set_option('display.max_rows', 500)
pd.set_option('display.max_columns', 500)
pd.set_option('display.width', 1000)

# Assign spreadsheet filename to `file`
qjt = 'Quality Job Tracker for JIRA.xlsx'
# Load spreadsheet
xlqjt = pd.ExcelFile(qjt)
# Print the sheet names
#print(xlqjt.sheet_names)
# Load a sheet into a DataFrame by name: df1
qjt_obj = xlqjt.parse('Quality Job Tracker for JIRA')

jira = 'ALL JIRA Bugs.xlsx'
# Load spreadsheet
xlqjt = pd.ExcelFile(jira)
# Print the sheet names
#print(xlqjt.sheet_names)
# Load a sheet into a DataFrame by name: df1
jira_obj = xlqjt.parse('ALL JIRA Bugs')

merged_file = jira_obj.merge(qjt_obj, on = "Primary", how = "left")

#fill in cells that were not merged
filled = merged_file.fillna('555')
# Get names of indexes for which column ISBN is NULL
indexNames = filled[filled['ISBN'] == '555'].index
#print(indexNames)
# Delete these row indexes from dataFrame with 555 in the ISBN field
merged_file.drop(indexNames, inplace=True)
merged_file.drop(['Freelancer Name', 'Input into JIRA', 'Service Line'] , axis='columns', inplace=True)
print(merged_file)
# group by Editorial and display distribution
print(f"********** Editorial grouping \n{merged_file['Editorial Quality Specialist'].value_counts()}")
# group by Primary and display distribution
print(f"********** Primary Grouping \n{merged_file['Primary'].value_counts()}")
# show matrix of NUll values "True or False"
print(f"********** False entries have NUll values \n{merged_file.isnull()}")
# show total number of NULL entries
print(f"********** SUM of Null {merged_file.isnull().sum().sum()}")
# Show NULLS per column
print(f"********** SUM of Null per Column\n{merged_file.isnull().sum()}")
#show NULLS per row
print(f"********** SUM of Null per Row")
for i in range(len(merged_file.index)):
    print("********** Nan in row ", i, " : ", merged_file.iloc[i].isnull().sum())