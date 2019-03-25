# Import pandas
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

'''
pd.set_option('display.max_rows', 500)
pd.set_option('display.max_columns', 500)
pd.set_option('display.width', 1000)

# read in spreadsheets and convert all data to text
qjt = 'Quality Job Tracker for JIRA.xlsx'
qjt_obj = pd.read_excel(qjt, dtype=str)
jira = 'ALL JIRA Bugs.xlsx'
jira_obj = pd.read_excel(jira, dtype=str)
# merged the 2 files based on "Primary" field match
merged_file = jira_obj.merge(qjt_obj, on="Primary", how="left")
print(merged_file.dtypes)
# fill in cells that were not merged
filled = merged_file.fillna('555')
# Get names of indexes for which column ISBN is NULL
indexNames = filled[filled['ISBN'] == '555'].index
# Delete these row indexes from dataFrame with 555 in the ISBN field
merged_file.drop(indexNames, inplace=True)
merged_file.drop(['Freelancer Name', 'Input into JIRA', 'Service Line'], axis='columns', inplace=True)
print(merged_file)
merged_file.to_csv('test.txt', sep='\t')
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
# show NULLS per row
print(f"********** SUM of Null per Row")
for i in range(len(merged_file.index)):
    print("********** Nan in row ", i, " : ", merged_file.iloc[i].isnull().sum())
'''

#xarr = [0,65,68,69,234,65,65,70,67,68,69,56,5,40,98,65,65,85,69,65,65,70,67,68,69,56,5,40,98,65,65,65,85,69,65,65,70,67,68,69,56,5,40,98,65]
import random
xarr = []
max=200
for i in range(1,max):
    x = random.randint(85,115)
    xarr.append(x)
for i in range(10):
    x = random.randint(1,max)
    y = random.randint(1,max)
    xarr[x] = y
print (f"Master = {xarr}")
print()
yarr = list(range(len(xarr)))
elements = np.array(xarr)
mean = np.mean(elements, axis=0)
sd = np.std(elements, axis=0)
print(f"mean = {mean}   dev={sd}  calc= {mean - (.2 * sd)}")
print()
#final_list = [x for x in xarr if (x > mean - .5 * sd)]
#final_list = [x for x in final_list if (x < mean + .5 * sd)]
final_list = []
temp_list = []
higher = []
lower = []
for x in xarr:
    if (x > mean - 2 * sd):
        temp_list.append(x)
    else:
        lower.append(x)

for x in temp_list:
    if (x < mean + 2 * sd):
        final_list.append(x)
    else:
        higher.append(x)

yarr1 = list(range(len(final_list)))
print(f"FINAL {len(final_list) / (len(xarr))} = {final_list}")
print()
print(f"Higher {len(higher) / (len(xarr))}   = {higher}")
print()
print(f"Lower {len(lower)/(len(xarr))} = {lower}")

#plt.gca().set_color_cycle(['red', 'green'])
#plt.plot(x, arr)

plt.xlabel("Time (s)")
plt.ylabel("Temp (F)")
plt.plot(yarr, xarr, yarr1, final_list)
plt.draw()
#plt.show(block=True)
print("---Plot graph finish---")
plt.ion()
plt.show()
plt.pause(1)
input("Press Enter")



