/**
 * @file wildcard-path-matcher.h
 * @author ofir iluz (iluzofir@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef WILDCARD_PATH_MATCHER_HPP_
#define WILDCARD_PATH_MATCHER_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace octo::wildcardmatching
{
class WildcardPathMatcher
{
  private:
    struct Wildcard
    {
        size_t offset, size;
        Wildcard(size_t begin, size_t end);
    };

  private:
    std::vector<std::string> wildcard_paths_;
    char folder_seperator_;
    bool allow_last_wildcard_as_many_paths_;

  private:
    /**
     * @brief
     * Evaluates the folder seperated based on the OS
     */
    void evaluate_os_folder_seperator();
    /**
     * @brief
     * Splits a string to a vector of strings by a given delimiter
     *
     * @param input
     * @param delimiter
     * @return std::vector<std::string>
     */
    std::vector<std::string> split_string_by_delimiter(const std::string& input, char delimiter) const;
    /**
     * @brief
     * Splits a given wildcard string to wildcard structs
     *
     * @param wildcard_str
     * @return std::vector<Wildcard>
     */
    std::vector<Wildcard> split_by_wildcards(const std::string& wildcard_str) const;
    /**
     * @brief
     * Compares the given two strings with wildcard possibility on the given string (*)
     *
     * @param wildcard_str
     * @param input_str
     * @return true
     * @return false
     */
    bool compare_validated_wildcard_strings(const std::string& wildcard_str, const std::string& input_str) const;
    /**
     * @brief
     * Compares the given input parts with the wildcard path
     *
     * @param input_path_parts
     * @param wildcard_path
     * @return true
     * @return false
     */
    bool compare_validated_wildcard_paths(const std::vector<std::string>& input_path_parts,
                                          const std::string& wildcard_path) const;
    /**
     * @brief
     * Handles the check for double wildcard or single wildcard at the end
     *
     * @param wildcard_path_parts
     * @param input_path_parts
     * @param wildcard_path_part_index
     * @param input_path_part_index
     * @return true
     * @return false
     */
    bool handle_double_wildcard_part_comparison(const std::vector<std::string>& wildcard_path_parts,
                                                const std::vector<std::string>& input_path_parts,
                                                int& wildcard_path_part_index,
                                                int& input_path_part_index) const;
    /**
     * @brief
     * Handles the check for normal part which is not double wildcard or single wildcard at the end
     *
     * @param wildcard_path_parts
     * @param input_path_parts
     * @param wildcard_path_part_index
     * @param input_path_part_index
     * @return true
     * @return false
     */
    bool handle_normal_part_comparison(const std::vector<std::string>& wildcard_path_parts,
                                       const std::vector<std::string>& input_path_parts,
                                       int& wildcard_path_part_index,
                                       int& input_path_part_index) const;
    /**
     * @brief
     * Makes the final decision if a match was found or not
     *
     * @param wildcard_path_parts
     * @param input_path_parts
     * @param wildcard_path_part_index
     * @param input_path_part_index
     * @return true
     * @return false
     */
    bool decide_if_match_is_found(const std::vector<std::string>& wildcard_path_parts,
                                  const std::vector<std::string>& input_path_parts,
                                  int& wildcard_path_part_index,
                                  int& input_path_part_index) const;
    /**
     * @brief
     * Checkes whether we can allow treating last part as a double wildcard while it being a single
     * For example if allowed then /home/\* will be good agasint /home/x/y/z
     *
     * @param wildcard_path_part
     * @param wildcard_path_parts
     * @param wildcard_path_part_index
     * @return true
     * @return false
     */
    bool should_allow_last_wildcard_as_many_paths(const std::string& wildcard_path_part,
                                                  const std::vector<std::string>& wildcard_path_parts,
                                                  size_t wildcard_path_part_index) const;

  public:
    /**
     * @brief
     * Construct a new Wildcard Path Matcher object
     *
     * @param allow_last_wildcard_as_many_paths
     */
    WildcardPathMatcher(bool allow_last_wildcard_as_many_paths = false);
    /**
     * @brief
     * Destroy the Wildcard Path Matcher object
     *
     */
    virtual ~WildcardPathMatcher();
    /**
     * @brief
     * Get the allow last wildcard as many paths object
     *
     * @return true
     * @return false
     */
    bool get_allow_last_wildcard_as_many_paths() const;
    /**
     * @brief
     * Set the allow last wildcard as many paths object
     *
     * @param allow_last_wildcard_as_many_paths
     */
    void set_allow_last_wildcard_as_many_paths(bool allow_last_wildcard_as_many_paths);
    /**
     * @brief
     * Get the folder seperator object
     *
     * @return char
     */
    char get_folder_seperator() const;
    /**
     * @brief
     * Set the folder seperator object
     *
     * @param folder_seperator
     */
    void set_folder_seperator(char folder_seperator);
    /**
     * @brief
     * Validates whether a string is a valid wildcard string
     * Meaning that it can contain a double wildcard standalone or a single wildcard standalone or within a string
     *
     * @param wildcard_path
     * @return true
     * @return false
     */
    bool validate_wildcard_path(const std::string& wildcard_path) const;
    /**
     * @brief
     *
     * @param wildcard_path
     * @return true
     * @return false
     */
    void add_wildcard_path(const std::string& wildcard_path);
    /**
     * @brief
     *
     * @param wildcard_path
     * @return true
     * @return false
     */
    void add_wildcard_paths(const std::vector<std::string>& wildcard_paths);
    /**
     * @brief
     *
     */
    void clean_wildcard_paths();
    /**
     * @brief
     * Get the wildcard paths object
     *
     * @return std::vector<std::string>
     */
    std::vector<std::string> get_wildcard_paths() const;
    /**
     * @brief
     * Checks whether a given input has a match against any of the wildcard paths added
     *
     * @param input
     * @return true
     * @return false
     */
    bool has_match(const std::string& input) const;
    /**
     * @brief
     * If a match exists between the input and the wildcard paths, will be returned
     * Otherwise empty string will be returned
     *
     * @param input
     * @return std::string
     */
    std::string get_wildcard_match(const std::string& input) const;
};
} // namespace octo::wildcardmatching
#endif