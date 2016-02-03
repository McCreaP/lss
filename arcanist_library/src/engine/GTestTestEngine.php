<?php

/**
 * Very basic 'GTest' unit test engine wrapper.
 */
final class GTestTestEngine extends ArcanistUnitTestEngine {

    private $projectRoot;

    public function getEngineConfigurationName() {
        return 'gtest';
    }

    public function shouldEchoTestResults() {
        return false;
    }

    protected function supportsRunAllTests() {
        return true;
    }

    public function run() {
        $working_copy = $this->getWorkingCopy();
        $this->projectRoot = $working_copy->getProjectRoot();

        $test_result = 'test_detail.xml';

        $build_future = $this->buildBuildFuture();
        $build_future->resolvex();
        $coverage_future = $this->buildCoverageFuture();
        try {
            $coverage_future->resolvex();
        } catch (CommandException $e) {
            exit($e->getStdout());
        }
        return $this->parseTestResults($test_result);
    }

    private function buildBuildFuture() {
        $cmd_line = "build.sh";
        return new ExecFuture('%C', $cmd_line);
    }

    private function buildCoverageFuture() {
        $cmd_line = "coverage.sh";
        return new ExecFuture('%C', $cmd_line);
    }

    private function parseTestResults($test_result) {
        $xunit_dom = new DOMDocument();
        $xunit_dom->loadXML(Filesystem::readFile($test_result));
        $results = array();
        $tests = $xunit_dom->getElementsByTagName('testcase');
        $coverage = $this->parseCoverage();
        foreach ($tests as $test) {
            $name = $test->getAttribute('name');
            $time = floatval($test->getAttribute('time'));
            $status = ArcanistUnitTestResult::RESULT_UNSOUND;
            switch ($test->getAttribute('status')) {
                case 'run':
                    if ($test->hasChildNodes())
                        $status = ArcanistUnitTestResult::RESULT_FAIL;
                    else
                        $status = ArcanistUnitTestResult::RESULT_PASS;
                    break;
                case 'notrun':
                    $status = ArcanistUnitTestResult::RESULT_SKIP;
                    break;
            }

            $result = new ArcanistUnitTestResult();
            $result->setName($name);
            $result->setDuration($time);
            $result->setCoverage($coverage);
            $result->setResult($status);
            $results[] = $result;
        }
        return $results;
    }

    private function parseCoverage() {
        $coverage = array();
        foreach ($this->rglob("*.gcov.html") as $coverage_path) {
            $coverage_html = new DOMDocument();
            $coverage_html->loadHTML(file_get_contents($coverage_path));;
            $xpath = new DOMXPath($coverage_html);
            $coverage_line = '';
            foreach ($xpath->query('//span[@class="lineNum"]') as $line_node) {
                $coverage_flag = 'N';
                $sibling = $line_node->nextSibling;
                if (get_class($sibling) == 'DOMElement') {
                    if ($sibling->getAttribute('class') == 'lineCov') {
                        $coverage_flag = 'C';
                    }
                    if ($sibling->getAttribute('class') == 'lineNoCov') {
                        $coverage_flag = 'U';
                    }
                }
                $coverage_line .= $coverage_flag;
            }
            $relative_path = substr($coverage_path, strlen('./coverage/html/'));
            $relative_path = substr($relative_path, 0, strlen($relative_path) - 10);
            $coverage[$relative_path] = $coverage_line;
        }
        return $coverage;
    }

    private function rglob($pattern) {
        $files = glob($pattern);
        foreach (glob(dirname($pattern) . '/*', GLOB_ONLYDIR | GLOB_NOSORT) as $dir) {
            $files = array_merge($files, $this->rglob($dir . '/' . basename($pattern)));
        }
        return $files;
    }
}
