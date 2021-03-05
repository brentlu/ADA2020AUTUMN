#!/usr/bin/python3
import subprocess

def test_run(index):
	ret = False

	# generate test data
	data_cmd = ['./party_data']

	data_p = subprocess.Popen(data_cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)
	data_outs, data_errs = data_p.communicate()
	if data_p.returncode != 0:
		print('%s fail' % (data_cmd))
		return ret

	# run ref program
	test_data = open('input-max.txt')

	ref_cmd = ['./party-1987']
	ref_p = subprocess.Popen(ref_cmd, stdin=test_data, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)
	ref_outs, ref_errs = ref_p.communicate()
	if ref_p.returncode != 0:
		print('%s fail' % (ref_cmd))
		return ret

	test_data.close()

	# run dut program
	test_data = open('input-max.txt')

	dut_cmd = ['./party']
	dut_p = subprocess.Popen(dut_cmd, stdin=test_data, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)
	dut_outs, dut_errs = dut_p.communicate()
	if dut_p.returncode != 0:
		print('%s fail' % (dut_cmd))
		return ret

	if (ref_outs == dut_outs):
		print('PASS: %d' %(index))
		ret = True
	else:
		print('FAIL: %d' % (index))
		print('ref_outs:');
		print('%s' % (ref_outs));
		print('dut_outs:');
		print('%s' % (dut_outs));

	test_data.close()

	return ret

def main():
	i = 0
	while True:
		i += 1
		ret = test_run(i)
		if (ret == False):
			print('Fail in %d test loop' % (i))
			break


if __name__ == '__main__':
	main()
